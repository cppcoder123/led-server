/*
 *
 */

#include <stdint.h>

#include "unix/constant.h"

#include "encode.h"
#include "invoke.h"
#include "poll.h"

#define ONCE_PER_SECOND 100

static uint8_t started = 0;

void poll_init ()
{
  started = 0;
}

static void poll_callback ()
{
  encode_msg_0 (MSG_ID_POLL, SERIAL_ID_TO_IGNORE);
}

void poll_start ()
{
  if (started != 0)
    return;

  started = 1;
  invoke_enable (INVOKE_ID_POLL, ONCE_PER_SECOND, poll_callback);
}

void poll_stop ()
{
  if (started == 0)
    return;

  invoke_disable (INVOKE_ID_POLL);
}
