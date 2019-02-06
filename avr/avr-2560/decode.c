/*
 *
 */

#include "mcu/constant.h"

#include "data-type.h"
#include "decode.h"
#include "encode.h"
#include "flush.h"
#include "spi.h"

#define IN_SIZE 10

#define CATCH_DECODED (1 << 0)
#define SIZE_DECODED (1 << 1)
#define SERIAL_DECODED (1 << 2)
#define MSG_ID_DECODED (1 << 3)

/*catch eye is found*/
static uint8_t decoded;
static uint8_t msg_size;
static uint8_t msg_serial;
static uint8_t msg_id;

static data_t in_buf[IN_SIZE];

static uint8_t is_decodable (data_t msg_id)
{
  /*fixme*/
  return 1;
}

static void decode ()
{

  switch (msg_id) {
  case MSG_ID_MONO_LED:         /* monochrome */
    {
        uint8_t status = (flush_push_mono (in_buf[0]) == 1)
          ? STATUS_SUCCESS : STATUS_FAIL;
        encode_msg_1 (MSG_ID_STATUS, msg_serial, status);
    }
    break;
  case MSG_ID_PING:
    encode_msg_1 (MSG_ID_STATUS, msg_serial, STATUS_SUCCESS);
    break;
  case MSG_ID_QUERY:
    // just ignore, other party tries to read smth
    break;
  default:
    encode_msg_1 (MSG_ID_STATUS, msg_serial, STATUS_UNKNOWN_MSG);
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
  /*header decode*/
  if (decoded == 0) {
    data_t symbol;
    if (spi_read_symbol (&symbol) == 0)
      return;                   /* wait */
    if (symbol != ID_CATCH_EYE)
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
    encode_msg_1 (MSG_ID_STATUS, 0, STATUS_HOLD_ON);
    return;                     /* wait */
  }
  if (spi_read_array (in_buf, msg_size - 2) == 0)
    return;                     /* wait */

  /* decode rest of msg*/
  decode ();

  decode_init ();
}
