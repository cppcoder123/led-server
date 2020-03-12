/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "buf.h"
#include "counter.h"
#include "debug.h"

#define MAX_ID COUNTER_5

#define ARRAY_SIZE (COUNTER_5 + 1)

#define PRESCALER_MASK (COUNTER_PRESCALER_1 | COUNTER_PRESCALER_8 \
  | COUNTER_PRESCALER_256)

#define COMPARE_A_8_BIT_FLAG (1 << 1)
#define COMPARE_A_16_BIT_FLAG (1 << 3)

/* either set "|=" or clear "&= ~" */
typedef void (*reg_access) (uint8_t/*counter-id*/, uint8_t/*value*/);

static counter_handle overflow[ARRAY_SIZE];
static counter_handle compare_a[ARRAY_SIZE];

static volatile struct buf_t handle_queue;

void counter_init ()
{
  for (uint8_t i = 0; i < ARRAY_SIZE; ++i) {
    /* prescaler[i] = COUNTER_PRESCALER_0; */
    overflow[i] = 0;
    compare_a[i] = 0;
  }

  buf_init (&handle_queue);
}

static uint8_t eight_bits (uint8_t id)
{
  return ((id == COUNTER_0) || (id == COUNTER_2)) ? 1 : 0;
}

static void tccra_set (uint8_t id, uint8_t value)
{
  switch (id) {
  case COUNTER_0:
    TCCR0A |= value;
    break;
  case COUNTER_1:
    TCCR1A |= value;
    break;
  case COUNTER_2:
    TCCR2A |= value;
    break;
  case COUNTER_3:
    TCCR3A |= value;
    break;
  case COUNTER_4:
    TCCR4A |= value;
    break;
  case COUNTER_5:
    TCCR5A |= value;
    break;
  default:
    break;
  }
}

static void tccra_clear (uint8_t id, uint8_t value)
{
  switch (id) {
  case COUNTER_0:
    TCCR0A &= ~value;
    break;
  case COUNTER_1:
    TCCR1A &= ~value;
    break;
  case COUNTER_2:
    TCCR2A &= ~value;
    break;
  case COUNTER_3:
    TCCR3A &= ~value;
    break;
  case COUNTER_4:
    TCCR4A &= ~value;
    break;
  case COUNTER_5:
    TCCR5A &= ~value;
    break;
  default:
    break;
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

void counter_enable (uint8_t id, uint8_t prescaler)
{
  if (id > MAX_ID)
    return;

  tccrb_set (id, prescaler & PRESCALER_MASK);
}

void counter_disable (uint8_t id)
{
  if (id > MAX_ID)
    return;

  tccrb_clear (id, PRESCALER_MASK);
}

/* void counter_prescaler (uint8_t id, uint8_t value) */
/* { */
/*   if (id > MAX_ID) */
/*     return; */

/*   prescaler[id] = value & PRESCALER_MASK; */
/* } */

static void timsk_set (uint8_t id, uint8_t value)
{
  switch (id) {
  case COUNTER_0:
    TIMSK0 |= value;
    break;
  case COUNTER_1:
    TIMSK1 |= value;
    break;
  case COUNTER_2:
    TIMSK2 |= value;
    break;
  case COUNTER_3:
    TIMSK3 |= value;
    break;
  case COUNTER_4:
    TIMSK4 |= value;
    break;
  case COUNTER_5:
    TIMSK5 |= value;
    break;
  default:
    break;
  }
}

static void timsk_clear (uint8_t id, uint8_t value)
{
  switch (id) {
  case COUNTER_0:
    TIMSK0 &= ~value;
    break;
  case COUNTER_1:
    TIMSK1 &= ~value;
    break;
  case COUNTER_2:
    TIMSK2 &= ~value;
    break;
  case COUNTER_3:
    TIMSK3 &= ~value;
    break;
  case COUNTER_4:
    TIMSK4 &= ~value;
    break;
  case COUNTER_5:
    TIMSK5 &= ~value;
    break;
  default:
    break;
  }
}

void counter_interrupt_enable (uint8_t id, uint8_t int_type, counter_handle fun)
{
  if ((id > MAX_ID)
      || ((int_type != COUNTER_INTERRUPT_OVERFLOW)
          && (int_type != COUNTER_INTERRUPT_COMPARE_A))
      || (fun == 0))
    return;

  counter_handle *array = overflow;
  uint8_t int_mask = 0;
  if (int_type == COUNTER_INTERRUPT_COMPARE_A) {
    array = compare_a;
    int_mask = (eight_bits (id) != 0)
      ? COMPARE_A_8_BIT_FLAG : COMPARE_A_16_BIT_FLAG;
  }

  *(array + id) = fun;

  reg_access reg_set = (eight_bits (id) != 0) ? &tccra_set : &tccrb_set;

  timsk_set (id, int_type);
  reg_set (id, int_mask);
}

void counter_interrupt_disable (uint8_t id, uint8_t int_type)
{
  reg_access reg_clear = (eight_bits (id) != 0) ? &tccra_clear : &tccrb_clear;
  uint8_t mask = (int_type == COUNTER_INTERRUPT_COMPARE_A)
    ? ((eight_bits (id) != 0) ? COMPARE_A_8_BIT_FLAG : COMPARE_A_16_BIT_FLAG)
    : 0;

  timsk_clear (id, int_type);
  reg_clear (id, mask);
}

void counter_set_register (uint8_t id, uint8_t reg, uint8_t low, uint8_t high)
{
  volatile uint8_t *reg_array[] =
    {
     &TCNT0, &TCNT0,            /* counter itself */
     &TCNT1L, &TCNT1H,
     &TCNT2, &TCNT2,
     &TCNT3L, &TCNT3H,
     &TCNT4L, &TCNT4H,
     &TCNT5L, &TCNT5H,
     &OCR0A, &OCR0A,            /*output compare a*/
     &OCR1AL, &OCR1AH,
     &OCR2A, &OCR2A,
     &OCR3AL, &OCR3AH,
     &OCR4AL, &OCR4AH,
     &OCR5AL, &OCR5AH,
     &OCR0B, &OCR0B,            /*output compare b*/
     &OCR1BL, &OCR1BH,
     &OCR2B, &OCR2B,
     &OCR3BL, &OCR3BH,
     &OCR4BL, &OCR4BH,
     &OCR5BL, &OCR5BH
  };

  const uint8_t shift = 12;

  uint8_t factor = (reg == COUNTER_OUTPUT_COMPARE_A) ? 1
    : (reg == COUNTER_OUTPUT_COMPARE_B) ? 2
    : 0;

  uint8_t reg_array_id = id + factor * shift;
  volatile uint8_t *lhs_low = reg_array[reg_array_id];
  volatile uint8_t *lhs_high = reg_array[reg_array_id + 1];

  *lhs_low = low;

  if (eight_bits (id) == 0)
    *lhs_high = high;
}

/*   switch (id) { */
/*   case COUNTER_0: */
/*     OCR0A = low; */
/*     break; */
/*   case COUNTER_1: */
/*     OCR1AL = low; */
/*     OCR1AH = high; */
/*     break; */
/*   case COUNTER_2: */
/*     OCR2A = low; */
/*     break; */
/*   case COUNTER_3: */
/*     OCR3AL = low; */
/*     OCR3AH = high; */
/*     break; */
/*   case COUNTER_4: */
/*     OCR4AL = low; */
/*     OCR4AH = high; */
/*     break; */
/*   case COUNTER_5: */
/*     OCR5AL = low; */
/*     OCR5AH = high; */
/*     break; */
/*   default: */
/*     break; */

/*   } */

/*   lhs_low = &TCNT0; */
/* } */


/* void counter_set_compare_a (uint8_t id, uint8_t low, uint8_t high) */
/* { */
/*   switch (id) { */
/*   case COUNTER_0: */
/*     OCR0A = low; */
/*     break; */
/*   case COUNTER_1: */
/*     OCR1AL = low; */
/*     OCR1AH = high; */
/*     break; */
/*   case COUNTER_2: */
/*     OCR2A = low; */
/*     break; */
/*   case COUNTER_3: */
/*     OCR3AL = low; */
/*     OCR3AH = high; */
/*     break; */
/*   case COUNTER_4: */
/*     OCR4AL = low; */
/*     OCR4AH = high; */
/*     break; */
/*   case COUNTER_5: */
/*     OCR5AL = low; */
/*     OCR5AH = high; */
/*     break; */
/*   default: */
/*     break; */
/*   } */
/* } */

/* void counter_set (uint8_t id, uint8_t low, uint8_t high) */
/* { */
/*   switch (id) { */
/*   case COUNTER_0: */
/*     TCNT0 = low; */
/*     break; */
/*   case COUNTER_1: */
/*     TCNT1L = low; */
/*     TCNT1H = high; */
/*     break; */
/*   case COUNTER_2: */
/*     TCNT2 = low; */
/*     break; */
/*   case COUNTER_3: */
/*     TCNT3L = low; */
/*     TCNT3H = high; */
/*     break; */
/*   case COUNTER_4: */
/*     TCNT4L = low; */
/*     TCNT4H = high; */
/*     break; */
/*   case COUNTER_5: */
/*     TCNT5L = low; */
/*     TCNT5H = high; */
/*     break; */
/*   default: */
/*     break; */
/*   } */
/* } */

/* void counter_get (uint8_t id, uint8_t *low, uint8_t *high) */
/* { */
/*   switch (id) { */
/*   case COUNTER_0: */
/*     *low = TCNT0; */
/*     break; */
/*   case COUNTER_1: */
/*     *low = TCNT1L; */
/*     *high = TCNT1H; */
/*     break; */
/*   case COUNTER_2: */
/*     *low = TCNT2; */
/*     break; */
/*   case COUNTER_3: */
/*     *low = TCNT3L; */
/*     *high = TCNT3H; */
/*     break; */
/*   case COUNTER_4: */
/*     *low = TCNT4L; */
/*     *high = TCNT4H; */
/*     break; */
/*   case COUNTER_5: */
/*     *low = TCNT5L; */
/*     *high = TCNT5H; */
/*     break; */
/*   default: */
/*     break; */
/*   } */
/* } */

static void interrupt_routine (uint8_t is_compare_a, uint8_t counter_id)
{
   if (is_compare_a != 0)
     counter_id += ARRAY_SIZE;

  buf_byte_fill (&handle_queue, counter_id);
}

ISR(TIMER0_OVF_vect)
{
  interrupt_routine (0, COUNTER_0);
}

ISR(TIMER1_OVF_vect)
{
  interrupt_routine (0, COUNTER_1);
}

ISR(TIMER2_OVF_vect)
{
  interrupt_routine (0, COUNTER_2);
}

ISR(TIMER3_OVF_vect)
{
  interrupt_routine (0, COUNTER_3);
}

ISR(TIMER4_OVF_vect)
{
  interrupt_routine (0, COUNTER_4);
}

ISR(TIMER5_OVF_vect)
{
  interrupt_routine (0, COUNTER_5);
}

ISR(TIMER0_COMPA_vect)
{
  interrupt_routine (1, COUNTER_0);
}

ISR(TIMER1_COMPA_vect)
{
  interrupt_routine (1, COUNTER_1);
}

ISR(TIMER2_COMPA_vect)
{
  interrupt_routine (1, COUNTER_2);
}

ISR(TIMER3_COMPA_vect)
{
  interrupt_routine (1, COUNTER_3);
}

ISR(TIMER4_COMPA_vect)
{
  interrupt_routine (1, COUNTER_4);
}

ISR(TIMER5_COMPA_vect)
{
  interrupt_routine (1, COUNTER_5);
}

void counter_try ()
{
  uint8_t id = 0;
  if (buf_byte_drain (&handle_queue, &id) == 0)
    return;

  counter_handle *array = overflow;
  if (id >= ARRAY_SIZE) {
    array = compare_a;
    id -= ARRAY_SIZE;
  }

  counter_handle fun = *(array + id);
  /* if (fun != 0) --- it doesn't work ---*/
  fun ();
}
