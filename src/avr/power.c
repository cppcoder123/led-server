/*
 *
 */

#include <avr/io.h>

/* #include "unix/constant.h" */

/* #include "debug.h" */

#include "counter.h"
#include "flush.h"
#include "power.h"

/* #define POWER_PIN PORTC7 */

#define TEN_PER_SECOND_LOW 100
#define TEN_PER_SECOND_HIGH 0
#define PER_SECOND_LOW 0
#define PER_SECOND_HIGH 61

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

  counter_prescaler (COUNTER_3, COUNTER_PRESCALER_1024);
  counter_interrupt (COUNTER_3,
                     COUNTER_INTERRUPT_COMPARE_A, &flush_enable_shift);
  counter_set_compare_a (COUNTER_3,
                         TEN_PER_SECOND_LOW, TEN_PER_SECOND_HIGH);
  counter_enable (COUNTER_3);
}

void power_down ()
{
  /* see power_up */
  /* PORTC |= (1 << POWER_PIN); */

  /*expect internal clock info, try to flush every second*/
  /*fixme: actually clock call should be passed here, but it is not implemented*/
  /*Note: clock function should call 'flush_enable_clear' at the end*/
  /* timer_enable (TIMER_ONE_PER_SECOND, &flush_enable_clear); */
}
