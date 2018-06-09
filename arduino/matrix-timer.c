/*
 * We are using Timer-0 to calculate delays
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "matrix-timer.h"

/*
 * 16 Mhz and 1024 prescaler => 64 * (10^-6) sec,
 * so if we want 0.1 sec interrupt, we need 156 ticks.
 * Let our timer go forward, so 256 - 156 => counter initial value is 100
 */
#define TIMER_INITIAL_VALUE 100

#define INTERRUPT_FLAG (1 << TOIE0)

static volatile uint8_t delay_factor;

void matrix_timer_init ()
{
  delay_factor = 0;

  /*
   * Default mode is increment
   * though we need to set it to zero to clear it after arduino loader
   */
  TCCR0A = 0;

  /*
   * Max clock prescaler = 1024
   */
  TCCR0B = (1 << CS02) | (1 << CS00);

  /*
   * Interrupt enabled when needed
   */
}

ISR (TIMER0_OVF_vect)
{
  if (delay_factor > 0)
    --delay_factor;

  TCNT0 = TIMER_INITIAL_VALUE;
}

static void timer_enable ()
{
  TCNT0 = TIMER_INITIAL_VALUE;
  /*
   * Enable overflow interrupt
   */
  TIMSK0 |= INTERRUPT_FLAG;
}

static void timer_disable ()
{
  /*
   * Disable overflow interrupt
   */
  TIMSK0 &= ~INTERRUPT_FLAG;
}

void matrix_timer_wait (uint8_t dot1_sec_delay)
{
  timer_enable ();
  while (delay_factor > 0)
    /*busy wait here*/
    ;
  timer_disable ();
}


