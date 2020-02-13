/*
 *
 */

#include "invoke.h"
#include "heartbeat.h"

#define PROVOKE_LIMIT 1

static uint8_t current_misses = 0;
static uint8_t max_misses = 0;
static heartbeat_callback report = 0;
static heartbeat_callback provoke = 0;

void heartbeat_init ()
{
  current_misses = 0;
  max_misses = 0;
  report = 0;
  provoke = 0;
}

void heartbeat_confirm ()
{
  current_misses = 0;
}

static void verify ()
{
  if (++current_misses >= max_misses) {
    heartbeat_cancel ();
    report ();
    return;
  }

  if (current_misses > PROVOKE_LIMIT)
    provoke ();
}

void heartbeat_start (uint8_t delay, uint8_t max,
		      heartbeat_callback report_cb,
		      heartbeat_callback provoke_cb)
{
  if ((report_cb == 0) || (provoke_cb == 0))
    return;

  current_misses = 0;
  max_misses = PROVOKE_LIMIT + max;
  report = report_cb;
  provoke = provoke_cb;

  invoke_enable (INVOKE_ID_HEARTBEAT, delay, &verify);
}

void heartbeat_cancel ()
{
  invoke_disable (INVOKE_ID_HEARTBEAT);
}
