/*
 * 
 * 
 * 
 */
#include <stdint.h>

#include "buffer.h"
#include "codec.h"
#include "matrix.h"
#include "parse.h"
#include "spi-write.h"
#include "uart-read.h"

static volatile struct buffer_t *read;

static volatile uint8_t msg_id;
static volatile uint8_t msg_size;
static volatile uint8_t msg_serial_id;

static volatile uint8_t own_serial_id;

void parse_init ()
{
  read = uart_read_get_buffer ();
  /* */
  msg_id = ID_INVALID_MSG;
  msg_size = 0;
  msg_serial_id = 0;
  own_serial_id = 0;
}

static void parse_body_0 ()
{
  switch (msg_id) {
  case ID_INIT:
    spi_write_initialize ();
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_OK);
    break;
  case ID_UNINIT:
    spi_write_uninitialize ();
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_OK);
    break;
  case ID_HANDSHAKE:
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_OK);
    break;
  default:
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_MSG_UNKNOWN_0);
    break;
  }
}

static void parse_body_1 (uint8_t data)
{
  switch (msg_id) {
  case ID_PIXEL_DELAY:
    matrix_pixel_delay (data);
    break;
  case ID_PHRASE_DELAY:
    matrix_phrase_delay (data);
    break;
  case ID_STABLE_DELAY:
    matrix_stable_delay (data);
    break;
  case ID_BRIGHTNESS:
    spi_write_brightness (data);
    break;
  default:
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_MSG_UNKNOWN_1);
    break;
  }
}

static void parse_body ()
{
  switch (msg_id) {
  case ID_INIT:
  case ID_UNINIT:
  case ID_HANDSHAKE:
    parse_body_0 ();
    break;
  case ID_PIXEL_DELAY:
  case ID_PHRASE_DELAY:
  case ID_STABLE_DELAY:
  case ID_BRIGHTNESS:
    {
      uint8_t *data = 0;
      if (buffer_get (read, ID_HEADER_SIZE, &data) == 0) {
        codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_BUFFER_CORRUPTED_1);
        return;
      }
      parse_body_1 (*data);
    }
    break;
  case ID_SUB_MATRIX:
    {
      if (msg_size <= 2) {
        /* one for type, at least one for data */
        codec_encode_1 (ID_STATUS,
                        msg_serial_id, ID_STATUS_SUB_MATRIX_TOO_SHORT);
        return;
      }

      uint8_t *matrix_type = 0;
      if (buffer_get (read, ID_HEADER_SIZE, &matrix_type) == 0) {
        /*we should not get here, size is double checked*/
        codec_encode_1 (ID_STATUS, msg_serial_id,
                        ID_STATUS_BUFFER_CORRUPTED_1);
        return;
      }

      if (matrix_update_start (*matrix_type) == 0) {
        codec_encode_1 (ID_STATUS, msg_serial_id,
                        ID_STATUS_SUB_MATRIX_UPDATE_START_FAILURE);
        return;
      }

      uint8_t *data = 0;
      if (buffer_get (read, ID_HEADER_SIZE + 1, &data) == 0) {
        codec_encode_1 (ID_STATUS, msg_serial_id,
                        ID_STATUS_BUFFER_CORRUPTED_2);
        return;
      }
      
      if (matrix_update (data, msg_size - 1) == 0) {
        codec_encode_1 (ID_STATUS, msg_serial_id,
                        ID_STATUS_SUB_MATRIX_UPDATE_FAILURE);
        return;
      }

      if (matrix_update_finish (*matrix_type) == 0) {
        codec_encode_1 (ID_STATUS, msg_serial_id,
                        ID_STATUS_SUB_MATRIX_UPDATE_FINISH_FAILURE);
        return;
      }
    }
    break;
  default:
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_MSG_UNKNOWN_H);
    break;
  }
}

void parse ()
{
  /*
   * 1. Try to read header first 
   * 2. If 1. is OK read body
   * 3. If 2. is OK drain buffer
   */

  while (buffer_is_drainable (read, ID_HEADER_SIZE) != 0) {
    uint8_t *data = 0;
    if (buffer_get (read, 0, &data) == 0) {
      codec_encode_1 (ID_STATUS, ++own_serial_id, ID_STATUS_BUFFER_CORRUPTED_0);
      uint8_t symbol;
      buffer_drain_symbol (read, &symbol);
      continue;
    }
    if (codec_decode_header (data, &msg_size, &msg_id, &msg_serial_id) == 0) {
      uint8_t symbol;
      buffer_drain_symbol (read, &symbol);
      codec_encode_1 (ID_HEADER_DECODE_FAILED, ++own_serial_id, symbol);
      continue;
    } 

    /* header is OK, check body */
    if (buffer_is_drainable (read, ID_HEADER_SIZE + msg_size) == 0) {
      /* body is not yet arrived */
      break;
    }

    /*header is OK and body length is OK*/
    parse_body ();

    if (buffer_drain (read, ID_HEADER_SIZE + msg_size) == 0) {
      codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_DRAIN_FAILURE);
      continue;
    }
  }
}
