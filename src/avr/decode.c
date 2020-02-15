/*
 *
 */

#include <stdint.h>

#include "unix/constant.h"

#include "clock.h"
#include "debug.h"
#include "decode.h"
#include "encode.h"
#include "flush.h"
#include "heartbeat.h"
#include "mode.h"
#include "spi.h"

#define IN_SIZE (LED_ARRAY_SIZE + MSG_OVERHEAD)

#define CATCH_DECODED (1 << 0)
#define SIZE_DECODED (1 << 1)
#define SERIAL_DECODED (1 << 2)
#define MSG_ID_DECODED (1 << 3)

/*catch eye is found*/
static uint8_t decoded;
static uint8_t msg_size;
static uint8_t msg_serial;
static uint8_t msg_id;

static uint8_t in_buf[IN_SIZE];

static uint8_t is_decodable (uint8_t msg_id)
{
  /*fixme*/
  return 1;
}

static void decode ()
{
  heartbeat_confirm ();

  uint8_t status = STATUS_FAIL;

  switch (msg_id) {
  case MSG_ID_LED_ARRAY:
    {
      /* debug */
      status = (flush_shift_data (in_buf, msg_size - 2) == 1)
        ? STATUS_SUCCESS : STATUS_FAIL;
      /* status = STATUS_SUCCESS; */
      encode_msg_2 (MSG_ID_STATUS, msg_serial, status, MSG_ID_LED_ARRAY);
      /*debug*/
      uint8_t buf_size = flush_shift_buffer_size ();
      if (buf_size <= 64)
        debug_2 (DEBUG_DECODE, 77, buf_size, msg_size - 2);
      /*debug*/
    }
    break;
  case MSG_ID_QUERY:
    /* just ignore, other party tries to read smth */
    break;
  case MSG_ID_SYNC_CLOCK:
    clock_set (in_buf[0], in_buf[1]);
    encode_msg_2 (MSG_ID_STATUS, msg_serial, STATUS_SUCCESS, MSG_ID_SYNC_CLOCK);
    break;
  case MSG_ID_VERSION:
    status = (in_buf[0] == PROTOCOL_VERSION) ? STATUS_SUCCESS : STATUS_FAIL;
    encode_msg_1 (MSG_ID_VERSION, msg_serial, status);
    mode_set (MODE_SLAVE);
    /* if (status == STATUS_SUCCESS) { */
    /*   flush_shift_enable (); */
    /* } */
    break;
  default:
    encode_msg_2 (MSG_ID_STATUS, msg_serial, STATUS_UNKNOWN_MSG, msg_id);
    break;
  }
}

void decode_init ()
{
  decoded = 0;
  msg_size = 0;
  msg_serial = 0;
  msg_id = MSG_ID_EMPTY;
}

/*
 * EYE-CATCH | SIZE | SERIAL | MSG_ID |  PAYLOAD...
 *
 * size = 1 (serial) + 1 (msg_id) + sizeof (PAYLOAD)
 */

void decode_try ()
{
  if (spi_read_size () == 0)
    return;

  /*header decode*/
  if (decoded == 0) {
    uint8_t symbol;
    if (spi_read_symbol (&symbol) == 0)
      return;                   /* wait */
    if (symbol != EYE_CATCH)
      return;                   /* ignore */
    decoded |= CATCH_DECODED;
  }
  if ((decoded & SIZE_DECODED) == 0) {
    if (spi_read_symbol (&msg_size) == 0)
      return;                   /* wait */
    decoded |= SIZE_DECODED;
  }
  if ((decoded & SERIAL_DECODED) == 0) {
    if (spi_read_symbol (&msg_serial) == 0)
      return;                   /* wait */
    decoded |= SERIAL_DECODED;
  }
  if ((decoded & MSG_ID_DECODED) == 0) {
    if (spi_read_symbol (&msg_id) == 0)
      return;                   /* wait */
    decoded |= MSG_ID_DECODED;
  }
  if (is_decodable (msg_id) == 0) {
    /* encode_msg_1 (MSG_ID_STATUS, 0, STATUS_HOLD_ON); */
    return;                     /* wait */
  }

  /* debug_0 (DEBUG_DECODE, DEBUG_1); */

  if (spi_read_array (in_buf, msg_size - 2) == 0)
    return;                     /* wait */

  /* decode rest of msg*/
  decode ();

  decode_init ();
}
