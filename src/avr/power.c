/*
 *
 */

#include <avr/io.h>

#include "counter.h"
#include "flush.h"
#include "power.h"

#define TEN_PER_SECOND_LOW 100
#define TEN_PER_SECOND_HIGH 0

#define PER_SECOND_LOW 0
#define PER_SECOND_HIGH 61

static uint8_t mode = POWER_MASTER;

static void start_master ()
{
  /* fixme */
}

static void stop_master ()
{
  /* fixme */
}

static void start_slave ()
{
  counter_prescaler (COUNTER_3, COUNTER_PRESCALER_1024);
  counter_interrupt (COUNTER_3,
                     COUNTER_INTERRUPT_COMPARE_A, &flush_enable_shift);
  counter_set_compare_a (COUNTER_3,
                         TEN_PER_SECOND_LOW, TEN_PER_SECOND_HIGH);
  counter_enable (COUNTER_3);
}

static void stop_slave ()
{
  counter_disable (COUNTER_3);
  counter_set_compare_a (COUNTER_3, 0xFF, 0xFF);
  counter_interrupt (COUNTER_3, COUNTER_INTERRUPT_COMPARE_A, 0);
  counter_prescaler (COUNTER_3, COUNTER_PRESCALER_0);
}

void power_init ()
{
  /* fixme: Configure power wire as output and set 0 there */
  mode = POWER_SLAVE;
  start_slave ();
}

void power_set_mode (uint8_t new_mode)
{
  if (new_mode == mode)
    return;

  if (new_mode == POWER_MASTER) {
    stop_slave ();
    start_master ();
  } else {
    stop_master ();
    start_slave ();
  }
}

uint8_t power_get_mode ()
{
  return mode;
}

#if 0
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
#endif
