/*
 *
 */

#include "constant.h"
#include "data-type.h"
#include "decode.h"
#include "encode.h"
#include "flush.h"
#include "spi.h"

#define IN_SIZE 10

/*catch eye is found*/
static uint8_t catched;
static uint8_t msg_id;
static uint8_t payload_size;

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
        uint8_t status = (flush_push_mono (in_buf[1]) == 1)
          ? STATUS_SUCCESS : STATUS_FAIL;
        encode_msg_1 (MSG_ID_STATUS, in_buf[0], status);
    }
    break;
  case MSG_ID_PING:
    encode_msg_1 (MSG_ID_STATUS, in_buf[0], STATUS_SUCCESS);
    break;
  case MSG_ID_QUERY:
    // just ignore, other party tries to read smth
    break;
  default:
    encode_msg_1 (MSG_ID_STATUS, in_buf[0], STATUS_UNKNOWN_MSG);
    break;
  }
}

void decode_init ()
{
  catched = 0;
  msg_id = MSG_ID_EMPTY;
  payload_size = 0;
}

/*
 * EYE-CATCH | MSG_ID | SIZE(PAYLOAD) + 1 | SERIAL_ID |  PAYLOAD...
 */

void decode_try ()
{
  /*header decode*/
  if (catched == 0) {
    data_t symbol;
    if (spi_read_symbol (&symbol) == 0)
      return;                   /* wait */
    if (symbol != ID_CATCH_EYE)
      return;                   /* ignore */
    catched = 1;
  }
  if (msg_id == MSG_ID_EMPTY) {
    if (spi_read_symbol (&msg_id) == 0)
      return;                   /* wait */
  }
  if (payload_size == 0) {
    if (spi_read_symbol (&payload_size) == 0)
      return;                   /* wait */
  }
  if (is_decodable (msg_id) == 0) {
    encode_msg_1 (MSG_ID_STATUS, 0, STATUS_HOLD_ON);
    return;                     /* wait */
  }
  if (spi_read_array (in_buf, payload_size) == 0)
    return;                     /* wait */

  /*payload decode*/
  decode ();

  decode_init ();
}
