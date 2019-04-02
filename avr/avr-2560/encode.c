/*
 *
 */

#include "mcu/constant.h"

#include "encode.h"
#include "postpone.h"

#define OUT_SIZE 10

static data_t out_buf[OUT_SIZE];

static uint8_t encode_msg (data_t msg_id, data_t serial_id, data_t size)
{
  uint8_t len = 0;

  out_buf[len] = EYE_CATCH;
  out_buf[++len] = size;
  out_buf[++len] = serial_id;
  out_buf[++len] = msg_id;

  return len;
}

void encode_msg_1 (data_t msg_id, data_t serial_id, data_t payload_1)
{
  uint8_t len = encode_msg (msg_id, serial_id, 3);
  out_buf[++len] = payload_1;

  postpone_message (out_buf, ++len, serial_id);
}

void encode_msg_2 (data_t msg_id, data_t serial_id,
                   data_t payload_1, data_t payload_2)
{
  uint8_t len = encode_msg (msg_id, serial_id, 4);

  out_buf[++len] = payload_1;
  out_buf[++len] = payload_2;

  postpone_message (out_buf, ++len, serial_id);
}
