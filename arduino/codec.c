/*
 *
 */
#include "device-id.h"

#include "codec.h"
#include "uart-write.h"

volatile struct uart_write_buffer *write;

static const uint16_t divider = (uint16_t) UINT8_MAX + 1;

void codec_init ()
{
  write = uart_write_get_buffer ();
}

static uint8_t encode_uint16 (uint16_t data, uint8_t msb)
{
  if (msb) {
    return (uint8_t) (data / divider);
  }

  return (uint8_t) (data % divider);
}

static uint16_t decode_uint16 (uint8_t lsb, uint8_t msb)
{
  return divider * (uint16_t) msb + (uint16_t) lsb;
}

void codec_encode_0 (uint8_t msg_id, uint16_t msg_serial_id)
{
  codec_encode_1 (msg_id, msg_serial_id, 0/*dummy*/);
}

void codec_encode_1 (uint8_t msg_id, uint16_t msg_serial_id, uint8_t data)
{
  /* 
   * check space first:
   * eye-catcher (1), size (1), msg-id (1), serial (2), data (1)
   */
  uint8_t msg_size = ID_FROM_ARDUINO_HEADER_SIZE + 1;
  uint8_t buffer_size = write->size + msg_size;
  if (buffer_size > UART_WRITE_BUFFER_MAX_SIZE) {
    /* how we should complain here? */
    return;
  }

  write->data[buffer_size] = ID_EYE_CATCH;
  write->data[++buffer_size] = msg_size;
  write->data[++buffer_size] = msg_id;
  write->data[++buffer_size] = encode_uint16 (msg_serial_id, 0);
  write->data[++buffer_size] = encode_uint16 (msg_serial_id, 1);
  write->data[++buffer_size] = data;
  /*advance buffer size*/
  ++buffer_size;

  uart_write_kick ();
}

uint8_t codec_decode_header (volatile uint8_t *src,
                             uint16_t *msg_size,
                             uint8_t *msg_id,
                             uint16_t *msg_serial_id)
{
  if (*src != ID_EYE_CATCH)
    return 0;

  uint16_t tmp_size = decode_uint16 (*(src + 1), *(src + 2));
  if (tmp_size > ID_MAX_MATRIX_SIZE + ID_TO_ARDUINO_HEADER_SIZE)
    return 0;

  *msg_size = tmp_size;
  *msg_id = *(src + 3);
  *msg_serial_id = decode_uint16 (*(src + 4), *(src + 5));

  return 1;
}
