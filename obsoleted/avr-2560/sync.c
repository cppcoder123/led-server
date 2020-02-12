/*
 *
 *
 */

/*
 * Use timer0 for sync.
 * This timer has 8 bits, that complicates its usage for longer
 * times like 1 second, but all other 16 bits timers are already taken.
 */

#include <stdint.h>
#include <util/atomic.h>

#include "unix/constant.h"

#include "encode.h"
#include "invoke.h"
#include "sync.h"

/* 1 second is 3906 */
#define TWO_SECONDS 200
#define CALLS_PER_MINUTE 30

#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24

static uint8_t hour = 0;
static uint8_t min = 0;

static uint8_t call_num = 0;

static void increment_minute ()
{
  ++min;
  if (min < MINUTES_PER_HOUR)
    return;

  min = 0;
  ++hour;
  if (hour < HOURS_PER_DAY)
    return;

  hour = 0;
}

static void sync_call ()
{
  ++call_num;
  if (call_num >= CALLS_PER_MINUTE) {
    call_num = 0;
    increment_minute ();
  }

}

void sync_init ()
{
  hour = 0;
  min = 0;
  call_num = 0;

  invoke_enable (INVOKE_ID_SYNC, TWO_SECONDS, sync_call);

  /*fixme*/
}

void sync_clock (uint8_t new_hour, uint8_t new_min)
{
  /*debug, find out discrepancy*/
#if 0
  uint8_t fraction_high = 0;
  uint8_t fraction_low = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    fraction_low = (uint8_t) (fraction & 0xFF);
    fraction_high = (uint8_t) ((fraction >> 8) & 0xFF);
  }
  encode_msg_8 (MSG_ID_SYNC_CLOCK, SERIAL_ID_TO_IGNORE,
                hour, min, fraction_high, fraction_low,
                0, 0, new_hour, new_min);
#endif
  /*debug*/

  hour = new_hour;
  min = new_min;

  call_num = 0;
}
