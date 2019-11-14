/*
 *
 */

#include <avr/interrupt.h>

#include "timer.h"

/*1024 prescaler => (4 mHz / 1024) = ~4 kHz*/
#define PRESCALER_MASK ((1 << CS10) | (1 << CS12))
#define TIMER_ENABLE TCCR1B |= PRESCALER_MASK
#define TIMER_DISABLE TCCR1B &= ~PRESCALER_MASK
/*we want a call 10 times per second => (15.6 kHz / 10) = 1560*/
/* #define TIMER_FACTOR 1562 */

static timer_function interrupt_function;

void timer_disable ()
{
  TIMER_DISABLE;
}

void timer_enable (uint16_t delay, timer_function fun)
{
  /* fixme */

  /*CTC mode with OCR1A*/
  TCCR1B |= (1 << WGM32);

  /* counter value when interrupt shoud occur */
  OCR1A = delay;

  /*enable output-compare A interrupt*/
  TIMSK3 |=  (1 << OCIE3A);

  /*all default*/
  /* TCCR1A = 0; */

  interrupt_function = fun;

  TIMER_ENABLE;
}

ISR (TIMER1_COMPA_vect)
{
  /*Just try to enable flush & shift*/
  (*interrupt_function) ();
}
