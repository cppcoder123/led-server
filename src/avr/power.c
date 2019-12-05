/*
 *
 */

#include <avr/io.h>

/* #include "unix/constant.h" */

/* #include "debug.h" */

#include "flush.h"
#include "power.h"
#include "timer.h"

#define POWER_PIN PORTC7

void power_init ()
{
  /*configure C7 as output */
  /* DDRC |= (1 << DDC7); */
}

void power_up ()
{
  /* assign 0 to switch on */
  /* PORTC &= ~(1 << POWER_PIN); */

  /* debug_0 (DEBUG_FLUSH, DEBUG_11); */

  /* expect stream from r-pi */
  timer_enable (TIMER_TEN_PER_SECOND, &flush_enable_shift);
}

void power_down ()
{
  /* see power_up */
  /* PORTC |= (1 << POWER_PIN); */

  /*expect internal clock info, try to flush every second*/
  /*fixme: actually clock call should be passed here, but it is not implemented*/
  /*Note: clock function should call 'flush_enable_clear' at the end*/
  timer_enable (TIMER_ONE_PER_SECOND, &flush_enable_clear);
}
