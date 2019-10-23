/*
 *
 */

#include "timer.h"


#define MAX_ID TIMER_5

#define ARRAY_SIZE (TIMER_5 + 1)

#define PRESCALER_MASK (TIMER_PRESCALER_1 | TIMER_PRESCALER_8 |
TIMER_PRESCALER_256)

static uint8_t prescaler[ARRAY_SIZE];

static timer_handle overflow[ARRAY_SIZE];
static timer_handle compare_a[ARRAY_SIZE];

void timer_init ()
{
  for (uint8_t i = 0; i < ARRAY_SIZE; ++i) {
    prescaler[i] = TIMER_PRESCALER_0;
    overflow[i] = 0;
    compare_a[i] = 0;
  }
}

static void tccrb_set(uint8_t id, uint8_t value)
{
  switch (id) {
  case TIMER_0:
    TCCR0B |= value;
    break;
  case TIMER_1:
    TCCR1B |= value;
    break;
  case TIMER_2:
    TCCR2B |= value;
    break;
  case TIMER_3:
    TCCR3B |= value;
    break;
  case TIMER_4:
    TCCR4B |= value;
    break;
  case TIMER_5:
    TCCR5B |= value;
    break;
  default:
    break;
  }
}

static void tccrb_clear(uint8_t id, uint8_t value)
{
  switch (id) {
  case TIMER_0:
    TCCR0B &= ~value;
    break;
  case TIMER_1:
    TCCR1B &= ~value;
    break;
  case TIMER_2:
    TCCR2B &= ~value;
    break;
  case TIMER_3:
    TCCR3B &= ~value;
    break;
  case TIMER_4:
    TCCR4B &= ~value;
    break;
  case TIMER_5:
    TCCR5B &= ~value;
    break;
  default:
    break;
  }
}

void timer_enable (uint8_t id)
{
  if (id > MAX_ID)
    return;

  tccrb_set (id, prescaler[id]);
}

void timer_disable (uint8_t id)
{
  if (id > MAX_ID)
    return;

  tccrb_clear (id, PRESCALER_MASK);
}

void timer_prescaler (uint8_t id, uint8_t value)
{
  if (id > MAX_ID)
    return;

  prescaler[id] = value & PRESCALER_MASK;
}

void timer_interrupt (uint8_t id, uint8_t int_type, timer_handle fun)
{
  if ((id > MAX_ID)
      || ((int_type != TIMER_INTERRUPT_OVERFLOW)
          && (int_type != TIMER_INTERRUPT_COMPARE_A)))
    return;

  timer_handle *array = (int_type == TIMER_INTERRUPT_OVERFLOW)
    ? overflow : compare_a;

  if (fun != 0)
    timsk_set (id, int_type);
  else
    timsk_clear (id, int_type);

  *(array + id) = fun;
}

void timer_set (uint8_t id, uint8_t low, uint8_t high)
{
  if ((id == TIMER_0) || (id == TIMER_2)) {
    if (id == TIMER_0)
      TCCNT0 = low;
    else
      TCCNT2 = low;
    return;
  }
  switch (id) {
  case TIMER_1:
    TCCNT1L = low;
    TCCNT1H = high;
    break;
  case TIMER_3:
    TCCNT3L = low;
    TCCNT3H = high;
    break;
  case TIMER_4:
    TCCNT4L = low;
    TCCNT4H = high;
    break;
  case TIMER_5:
    TCCNT5L = low;
    TCCNT5H = high;
    break;
  default:
    break;
  }
}

void timer_get (uint8_t id, uint8_t *low, uint8_t *high)
{
  if ((id == TIMER_0) || (id == TIMER_2)) {
    if (id == TIMER_0)
      *low = TCCNT0;
    else
      *low = TCCNT2;
    return;
  }
  switch (id) {
  case TIMER_1:
    *low = TCCNT1L;
    *high = TCCNT1H;
    break;
  case TIMER_3:
    *low = TCCNT3L;
    *high = TCCNT3H;
    break;
  case TIMER_4:
    *low = TCCNT4L;
    *high = TCCNT4H;
    break;
  case TIMER_5:
    *low = TCCNT5L;
    *high = TCCNT5H;
    break;
  default:
    break;
  }
}

ISR(TIMER0_OVF_vect)
{
  timer_handle handle = overflow[TIMER_0];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER1_OVF_vect)
{
  timer_handle handle = overflow[TIMER_1];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER2_OVF_vect)
{
  timer_handle handle = overflow[TIMER_2];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER3_OVF_vect)
{
  timer_handle handle = overflow[TIMER_3];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER4_OVF_vect)
{
  timer_handle handle = overflow[TIMER_4];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER5_OVF_vect)
{
  timer_handle handle = overflow[TIMER_5];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER0_COMPA)
{
  timer_handle handle = compare_a[TIMER_0];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER1_COMPA)
{
  timer_handle handle = compare_a[TIMER_1];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER2_COMPA)
{
  timer_handle handle = compare_a[TIMER_2];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER3_COMPA)
{
  timer_handle handle = compare_a[TIMER_3];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER4_COMPA)
{
  timer_handle handle = compare_a[TIMER_4];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}

ISR(TIMER5_COMPA)
{
  timer_handle handle = compare_a[TIMER_5];

  if (handle == 0) {
    /*send err msg*/
    return;
  }

  handle();
}
