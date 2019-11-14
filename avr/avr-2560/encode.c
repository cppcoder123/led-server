/*
 *
 */

#include "unix/constant.h"

#include "encode.h"
#include "postpone.h"

#define OUT_SIZE 15

static uint8_t out_buf[OUT_SIZE];

static uint8_t encode_msg (uint8_t msg_id, uint8_t serial_id, uint8_t size)
{
  uint8_t len = 0;

  out_buf[len] = EYE_CATCH;
  out_buf[++len] = size;
  out_buf[++len] = serial_id;
  out_buf[++len] = msg_id;

  return len;
}

void encode_msg_1 (uint8_t msg_id, uint8_t serial_id, uint8_t payload_1)
{
  uint8_t len = encode_msg (msg_id, serial_id, 3);
  out_buf[++len] = payload_1;

  postpone_message (out_buf, ++len, serial_id);
}

void encode_msg_2 (uint8_t msg_id, uint8_t serial_id,
                   uint8_t payload_1, uint8_t payload_2)
{
  uint8_t len = encode_msg (msg_id, serial_id, 4);

  out_buf[++len] = payload_1;
  out_buf[++len] = payload_2;

  postpone_message (out_buf, ++len, serial_id);
}

void encode_msg_3 (uint8_t msg_id, uint8_t serial_id,
                   uint8_t payload_1, uint8_t payload_2, uint8_t payload_3)
{
  uint8_t len = encode_msg (msg_id, serial_id, 5);

  out_buf[++len] = payload_1;
  out_buf[++len] = payload_2;
  out_buf[++len] = payload_3;

  postpone_message (out_buf, ++len, serial_id);
}

void encode_msg_4 (uint8_t msg_id, uint8_t serial_id,
                   uint8_t payload_1, uint8_t payload_2,
                   uint8_t payload_3, uint8_t payload_4)
{
  uint8_t len = encode_msg (msg_id, serial_id, 6);

  out_buf[++len] = payload_1;
  out_buf[++len] = payload_2;
  out_buf[++len] = payload_3;
  out_buf[++len] = payload_4;

  postpone_message (out_buf, ++len, serial_id);
}

void encode_msg_8 (uint8_t msg_id, uint8_t serial_id,
                   uint8_t payload_1, uint8_t payload_2, uint8_t payload_3, uint8_t payload_4,
                   uint8_t payload_5, uint8_t payload_6, uint8_t payload_7, uint8_t payload_8)
{
  uint8_t len = encode_msg (msg_id, serial_id, 10);

  out_buf[++len] = payload_1;
  out_buf[++len] = payload_2;
  out_buf[++len] = payload_3;
  out_buf[++len] = payload_4;
  out_buf[++len] = payload_5;
  out_buf[++len] = payload_6;
  out_buf[++len] = payload_7;
  out_buf[++len] = payload_8;

  postpone_message (out_buf, ++len, serial_id);
}
