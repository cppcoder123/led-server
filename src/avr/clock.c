/*
 *
 *
 */

/*
 * Use timer0 for clock.
 * This timer has 8 bits, that a bit complicates its usage for longer
 * times like 1 second, but all other 16 bits timers are already taken.
 */

#include <stdint.h>
#include <util/atomic.h>

#include "unix/constant.h"

#include "clock.h"
#include "encode.h"
#include "counter.h"

/* Clock is 4Mhz, prescaler is 1024 => counter clock is ~3906 Hz */
/* if we will use 55 as compare__a_value then we will get ~71.023 */
/* interrupts per second */
#define COUNTER_COMPARE_A_VALUE 55

/*How many fractions we have in a second?*/
/* */
/* 71.023 * 60 = 4261.3636xxx*/
/* if we will use 4261 fraction per minute, we will get 0.36 sec */
/* error per minute, 22 sec per hour, 3 min during 8 hours => should be suitable */
#define FRACTION_PER_MINUTE 4261

#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24

static uint8_t hour = 0;
static uint8_t min = 0;
static volatile uint16_t fraction = 0;    /* fraction of a second */

static void clock_interrupt ()
{
  ++fraction;
}

void clock_init ()
{
  hour = 0;
  min = 0;

  counter_prescaler (COUNTER_0, COUNTER_PRESCALER_1024);
  counter_interrupt (COUNTER_0, COUNTER_INTERRUPT_COMPARE_A, &clock_interrupt);
  counter_set_compare_a (COUNTER_0, COUNTER_COMPARE_A_VALUE, 0/*not used*/);
  counter_enable (COUNTER_0);

  /*fixme*/
}

void clock_try ()
{
  uint8_t go_ahead = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if (fraction >= FRACTION_PER_MINUTE) {
      fraction = 0;
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

void clock_sync (uint8_t new_hour, uint8_t new_min)
{
  /*debug, find out discrepancy*/
  uint8_t fraction_high = 0;
  uint8_t fraction_low = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    fraction_low = (uint8_t) (fraction & 0xFF);
    fraction_high = (uint8_t) ((fraction >> 8) & 0xFF);
  }
  encode_msg_4 (MSG_ID_CLOCK_SYNC, SERIAL_ID_TO_IGNORE,
                hour, min, fraction_high, fraction_low);
  /*debug*/

  hour = new_hour;
  min = new_min;
  fraction = 0;
}
