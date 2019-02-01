/*
 *
 */

#include <avr/interrupt.h>

#include "flush.h"
#include "flush-timer.h"
#include "ring.h"


/*1024 prescaler => (16 mHz / 1024) = ~15.6 kHz*/
#define PRESCALER_MASK ((1 << CS10) | (1 << CS12))
#define TIMER_ENABLE TCCR1B |= PRESCALER_MASK
#define TIMER_DISABLE TCCR1B &= ~PRESCALER_MASK
/*we want a call 10 times per second => (15.6 kHz / 10) = 1560*/
#define TIMER_FACTOR 1562

void flush_timer_init ()
{
  /* fixme */

  /*all default*/
  /* TCCR1A = 0; */

  /*CTC mode with OCR1A*/
  TCCR1B |= (1 << WGM12);

  /* counter value when interrupt shoud occur */
  OCR1A = TIMER_FACTOR;

  /*enable output-compare A interrupt*/
  TIMSK1 |=  (1 << OCIE1A);

  TIMER_ENABLE;
}

ISR (TIMER1_COMPA_vect)
{
  /*Just try to enable flush & shift*/
  flush_enable (FLUSH_SHIFT);
}
