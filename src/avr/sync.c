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
#include "counter.h"
#include "sync.h"

/* 
 * Clock is 4Mhz, prescaler is 1024 => counter clock is ~3906 Hz 
 * The period will be 0,000256 sec. 
 * the goal is 60 sec, so the factor is equal to 234375.
 * 234375 = 5*5*5*5*5*5*5*3, max value close to 255 is 5*5*5 = 125
 * The fraction value is 5*5*5*5*3 = 1875
 */
#define COUNTER_COMPARE_A_VALUE 125
#define FRACTIONS_PER_MINUTE 1875

#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24

static uint8_t hour = 0;
static uint8_t min = 0;
static volatile uint16_t fraction = 0;    /* fraction of a second */

void sync_interrupt ()
{
  ++fraction;
}

void sync_init ()
{
  hour = 0;
  min = 0;
  fraction = 0;

  counter_prescaler (COUNTER_0, COUNTER_PRESCALER_1024);
  counter_interrupt (COUNTER_0, COUNTER_INTERRUPT_COMPARE_A, &sync_interrupt);
  counter_set_compare_a (COUNTER_0, COUNTER_COMPARE_A_VALUE, 0/*not used*/);
  counter_enable (COUNTER_0);

  /*fixme*/
}

void sync_try ()
{
  uint8_t go_ahead = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if (fraction >= FRACTIONS_PER_MINUTE) {
      fraction -= FRACTIONS_PER_MINUTE;
      go_ahead = 1;
    }
  }

  if (go_ahead == 0)
    return;

  ++min;
  if (min < MINUTES_PER_HOUR)
    return;

  min = 0;
  ++hour;
  if (hour < HOURS_PER_DAY)
    return;

  hour = 0;
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
  fraction = 0;
}
