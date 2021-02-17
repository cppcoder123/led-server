/*
 *
 */

#include "const/constant.h"

#include "at.h"
#include "debug.h"
#include "cron.h"

#define AT_DELAY 200         /* ~ 1 sec */

static uint8_t current[AT_MAX];
static uint8_t max[AT_MAX];
static at_callback callback[AT_MAX];

void at_init ()
{
  for (uint8_t i = 0; i < AT_MAX; ++i) {
    current[i] = 0;
    max[i] = 0;
    callback[i] = 0;
  }
}

static uint8_t empty ()
{
  for (uint8_t id = 0; id < AT_MAX; ++id)
    if (callback[id] != 0)
      return 0;

  return 1;
}

static void verify ()
{
  for (uint8_t id = 0; id < AT_MAX; ++id) {
    if (callback[id] == 0)
      continue;
    if (++current[id] >= max[id]) {
      callback[id] ();
      callback[id] = 0;
      if (empty () != 0)
        cron_disable (CRON_ID_AT);
    }
  }
}

void at_schedule (uint8_t id, uint8_t delay, at_callback cb)
{
  if ((id >= AT_MAX) || (cb == 0))
    return;

  /* debug_0 (DEBUG_BUZZ, 31); */

  /* uint8_t it_was_empty = empty (); */

  /* debug_1 (DEBUG_BUZZ, 11, it_was_empty); */

  current[id] = 0;
  max[id] = delay;
  callback[id] = cb;

  /* if (it_was_empty != 0) */
  /* debug_0 (DEBUG_BUZZ, 32); */
  cron_enable (CRON_ID_AT, AT_DELAY, &verify);
  /* debug_0 (DEBUG_BUZZ, 33); */
}

void at_postpone (uint8_t id)
{
  if (id >= AT_MAX)
    return;

  current[id] = 0;
}

void at_cancel (uint8_t id)
{
  if (id >= AT_MAX)
    return;

  callback[id] = 0;
  if (empty () != 0)
    cron_disable (CRON_ID_AT);
}

uint8_t at_empty (uint8_t id)
{
  if (id >= AT_MAX)
    /* don't schedule with wrong id */
    return 0;

  return (callback[id] == 0) ? 1 : 0;
}
