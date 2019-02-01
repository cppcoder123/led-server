/*
 *
 */

#include "constant.h"
#include "encode.h"
#include "spi.h"

#define OUT_SIZE 10

static data_t out_buf[OUT_SIZE];

static uint8_t encode_msg (data_t msg_id, data_t serial_id, data_t size)
{
  uint8_t len = 0;

  out_buf[len] = ID_CATCH_EYE;
  out_buf[++len] = msg_id;
  out_buf[++len] = size;
  out_buf[++len] = serial_id;

  return len;
}

uint8_t encode_msg_1 (data_t msg_id, data_t serial_id, data_t payload_1)
{
  uint8_t len = encode_msg (msg_id, serial_id, 2);
  out_buf[++len] = payload_1;

  return spi_write_array (out_buf, ++len);
}

uint8_t encode_msg_2 (data_t msg_id, data_t serial_id,
                      data_t payload_1, data_t payload_2)
{
  uint8_t len = encode_msg (msg_id, serial_id, 3);

  out_buf[++len] = payload_1;
  out_buf[++len] = payload_2;

  return spi_write_array (out_buf, ++len);
}
