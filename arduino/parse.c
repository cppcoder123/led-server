/*
 * 
 * 
 * 
 */
#include <stdint.h>

#include "codec.h"
#include "matrix.h"
#include "parse.h"
#include "spi-write.h"
#include "uart-read.h"
#include "uart-write.h"

volatile struct uart_read_buffer *read;
volatile struct uart_write_buffer *write;

uint16_t msg_size;
uint8_t msg_id;
uint16_t msg_serial_id;

uint16_t own_serial_id;


static void parse_body ()
{
  uint16_t bytes_available = read->size - read->current;
  if (msg_size > bytes_available)
    return;

  switch (msg_id) {
  case ID_INIT:
    spi_write_init ();
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_OK);
    break;
  case ID_UNINIT:
    spi_write_uninit ();
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_OK);
    break;
  case ID_HANDSHAKE:
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_OK);
    break;
  case ID_MATRIX:
    {
      uint8_t update_matrix = matrix_write_prepare (msg_size);
      if (update_matrix) {
        for (uint16_t i = read->current; i < read->current + msg_size; ++i) {
          matrix_write (read->data + i);
        }
      }
      uint8_t status_value = (update_matrix) ? ID_STATUS_OK : ID_STATUS_THROTTLE;
      codec_encode_1 (ID_STATUS, msg_serial_id, status_value);
    }
    break;
  case ID_SHIFT_DELAY:
    matrix_shift_delay (read->data[read->current], read->data[read->current + 1]);
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_OK);
    break;
  case ID_STABLE_DELAY:
    matrix_stable_delay (read->data[read->current]);
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_OK);
    break;
  case ID_BRIGHTNESS:
    spi_write_brightness (read->data[read->current]);
    codec_encode_1 (ID_STATUS, msg_serial_id, ID_STATUS_OK);
    break;
  default:
    codec_encode_1 (ID_UNKNOWN_MSG, msg_serial_id, msg_id);
    break;
  }

  read->current += msg_size;
}

void parse ()
{
  /*
   * current should be less than size
   */
  while (read->current < read->size) {
    if (msg_id == ID_UNKNOWN_MSG) {
      if (read->data[read->current] != ID_EYE_CATCH) {
        codec_encode_1 (ID_MISSING_EYE_CATCH, ++own_serial_id,
                        read->data[read->current]);
        ++(read->current);
        continue;
      }
      if (read->size - read->current >= ID_TO_ARDUINO_HEADER_SIZE) {
        if (codec_decode_header (read->data + read->current,
                                 &msg_size, &msg_id, &msg_serial_id) == 0) {
          codec_encode_0 (ID_HEADER_DECODE_FAILED, ++own_serial_id);
          msg_id = ID_UNKNOWN_MSG;
          ++(read->current);
          continue;
        }
        read->current += ID_TO_ARDUINO_HEADER_SIZE;
        /*
         * We have valid msg-id, size, serial id here,
         * we can parse empty body message
         */
        parse_body ();
      }
    } else {                    /* header already parsed */
      parse_body ();
    }
  }
}
