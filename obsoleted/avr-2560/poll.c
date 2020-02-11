/*
 *
 */

#include <stdint.h>

#include "unix/constant.h"

#include "encode.h"
#include "invoke.h"
#include "poll.h"

#define ONCE_PER_SECOND 100

enum {
  ENABLED = (1 << 0),           /* else disabled */
  STARTED = (1 << 1),           /* else stopped */
};

static uint8_t flag = 0;

void poll_init ()
{
  flag = 0;
}

void poll_enable ()
{
  return;
  flag |= ENABLED;
}

void poll_disable ()
{
  return;
  flag &= ~ENABLED;

  poll_stop ();
}

/* static void poll_callback () */
/* { */
/*   return; */
/*   encode_msg_0 (MSG_ID_POLL, SERIAL_ID_TO_IGNORE); */
/* } */

void poll_start ()
{
  return;

  if ((flag & STARTED) != 0)
    return;

  flag |= STARTED;
  /* invoke_enable (INVOKE_ID_POLL, ONCE_PER_SECOND, poll_callback); */
}

void poll_stop ()
{
  return;
  if ((flag & STARTED) == 0)
    return;

  flag &= ~STARTED;

  /* invoke_disable (INVOKE_ID_POLL); */
}
