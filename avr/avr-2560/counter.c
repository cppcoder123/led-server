/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "counter.h"


#define MAX_ID COUNTER_5

#define ARRAY_SIZE (COUNTER_5 + 1)

#define PRESCALER_MASK (COUNTER_PRESCALER_1 | COUNTER_PRESCALER_8 \
  | COUNTER_PRESCALER_256)

static uint8_t prescaler[ARRAY_SIZE];

static counter_handle overflow[ARRAY_SIZE];
static counter_handle compare_a[ARRAY_SIZE];

void counter_init ()
{
  for (uint8_t i = 0; i < ARRAY_SIZE; ++i) {
    prescaler[i] = COUNTER_PRESCALER_0;
    overflow[i] = 0;
    compare_a[i] = 0;
  }
}

static void tccrb_set(uint8_t id, uint8_t value)
{
  switch (id) {
  case COUNTER_0:
    TCCR0B |= value;
    break;
  case COUNTER_1:
    TCCR1B |= value;
    break;
  case COUNTER_2:
    TCCR2B |= value;
    break;
  case COUNTER_3:
    TCCR3B |= value;
    break;
  case COUNTER_4:
    TCCR4B |= value;
    break;
  case COUNTER_5:
    TCCR5B |= value;
    break;
  default:
    break;
  }
}

static void tccrb_clear(uint8_t id, uint8_t value)
{
  switch (id) {
  case COUNTER_0:
    TCCR0B &= ~value;
    break;
  case COUNTER_1:
    TCCR1B &= ~value;
    break;
  case COUNTER_2:
    TCCR2B &= ~value;
    break;
  case COUNTER_3:
    TCCR3B &= ~value;
    break;
  case COUNTER_4:
    TCCR4B &= ~value;
    break;
  case COUNTER_5:
    TCCR5B &= ~value;
    break;
  default:
    break;
  }
}

void counter_enable (uint8_t id)
{
  if (id > MAX_ID)
    return;

  tccrb_set (id, prescaler[id]);
}

void counter_disable (uint8_t id)
{
  if (id > MAX_ID)
    return;

  tccrb_clear (id, PRESCALER_MASK);
}

void counter_prescaler (uint8_t id, uint8_t value)
{
  if (id > MAX_ID)
    return;

  prescaler[id] = value & PRESCALER_MASK;
}

static void timsk_set (uint8_t id, uint8_t flag)
{
  // fixme
}

static void timsk_clear (uint8_t id, uint8_t flag)
{
  // fixme
}

void counter_interrupt (uint8_t id, uint8_t int_type, counter_handle fun)
{
  if ((id > MAX_ID)
      || ((int_type != COUNTER_INTERRUPT_OVERFLOW)
          && (int_type != COUNTER_INTERRUPT_COMPARE_A)))
    return;

  counter_handle *array = (int_type == COUNTER_INTERRUPT_OVERFLOW)
    ? overflow : compare_a;

  if (fun != 0)
    timsk_set (id, int_type);
  else
    timsk_clear (id, int_type);

  *(array + id) = fun;
}

void counter_set (uint8_t id, uint8_t low, uint8_t high)
{
  if ((id == COUNTER_0) || (id == COUNTER_2)) {
    if (id == COUNTER_0)
      TCNT0 = low;
    else
      TCNT2 = low;
    return;
  }
  switch (id) {
  case COUNTER_1:
    TCNT1L = low;
    TCNT1H = high;
    break;
  case COUNTER_3:
    TCNT3L = low;
    TCNT3H = high;
    break;
  case COUNTER_4:
    TCNT4L = low;
    TCNT4H = high;
    break;
  case COUNTER_5:
    TCNT5L = low;
    TCNT5H = high;
    break;
  default:
    break;
  }
}

void counter_get (uint8_t id, uint8_t *low, uint8_t *high)
{
  if ((id == COUNTER_0) || (id == COUNTER_2)) {
    if (id == COUNTER_0)
      *low = TCNT0;
    else
      *low = TCNT2;
    return;
  }
  switch (id) {
  case COUNTER_1:
    *low = TCNT1L;
    *high = TCNT1H;
    break;
  case COUNTER_3:
    *low = TCNT3L;
    *high = TCNT3H;
    break;
  case COUNTER_4:
    *low = TCNT4L;
    *high = TCNT4H;
    break;
  case COUNTER_5:
    *low = TCNT5L;
    *high = TCNT5H;
    break;
  default:
    break;
  }
}

ISR(TIMER0_OVF_vect)
{
  counter_handle handle = overflow[COUNTER_0];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER1_OVF_vect)
{
  counter_handle handle = overflow[COUNTER_1];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER2_OVF_vect)
{
  counter_handle handle = overflow[COUNTER_2];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER3_OVF_vect)
{
  counter_handle handle = overflow[COUNTER_3];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER4_OVF_vect)
{
  counter_handle handle = overflow[COUNTER_4];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER5_OVF_vect)
{
  counter_handle handle = overflow[COUNTER_5];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER0_COMPA_vect)
{
  counter_handle handle = compare_a[COUNTER_0];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

/* fixme: remove, redefition */
/* ISR(TIMER1_COMPA_vect) */
/* { */
/*   counter_handle handle = compare_a[COUNTER_1]; */

/*   if (handle == 0) { */
/*     /\*send err msg*\/ */
/*     return; */
/*   } */

/*   handle(); */
/* } */

ISR(TIMER2_COMPA_vect)
{
  counter_handle handle = compare_a[COUNTER_2];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER3_COMPA_vect)
{
  counter_handle handle = compare_a[COUNTER_3];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER4_COMPA_vect)
{
  counter_handle handle = compare_a[COUNTER_4];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER5_COMPA_vect)
{
  counter_handle handle = compare_a[COUNTER_5];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}
