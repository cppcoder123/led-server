/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "buf.h"
#include "counter.h"
#include "debug.h"

#define MAX_ID COUNTER_5

#define PRESCALER_MASK (COUNTER_PRESCALER_1 | COUNTER_PRESCALER_8 \
  | COUNTER_PRESCALER_256)


/* enable something */
#define FLAG_ENABLE_COMPARE_A_8 (1 << 1)
#define FLAG_ENABLE_COMPARE_A_16 (1 << 3)
/* enable interrupt */
#define FLAG_INTERRUPT_COMPARE_A (1 << 1)

enum {
  CONTROL_REGISTER_A,
  CONTROL_REGISTER_B,
  CONTROL_REGISTER_INTERRUPT,
};

static counter_handle compare_a[MAX_ID + 1];

static volatile struct buf_t handle_queue;

void counter_init ()
{
  for (uint8_t i = 0; i <= MAX_ID; ++i)
    compare_a[i] = 0;

  buf_init (&handle_queue);
}

static uint8_t eight_bits (uint8_t id)
{
  return ((id == COUNTER_0) || (id == COUNTER_2)) ? 1 : 0;
}

static volatile uint8_t* register_get (uint8_t counter_id, uint8_t register_id)
{
  volatile uint8_t *reg_array[] =
    {
      &TCCR0A, &TCCR1A, &TCCR2A, &TCCR3A, &TCCR4A, &TCCR5A,
      &TCCR0B, &TCCR1B, &TCCR2B, &TCCR3B, &TCCR4B, &TCCR5B,
      &TIMSK0, &TIMSK1, &TIMSK2, &TIMSK3, &TIMSK4, &TIMSK5, 
    };
  const uint8_t factor = (register_id == CONTROL_REGISTER_B) ? 1
    : (register_id == CONTROL_REGISTER_INTERRUPT) ? 2
    : 0;

  return reg_array[counter_id + (MAX_ID + 1) * factor];
}

static void register_set (uint8_t counter_id,
                          uint8_t register_id, uint8_t value)
{
  volatile uint8_t *reg = register_get (counter_id, register_id);
  *reg |= value;
}

static void register_clear (uint8_t counter_id,
                            uint8_t register_id, uint8_t value)
{
  volatile uint8_t *reg = register_get (counter_id, register_id);
  *reg &= ~value;
}

void counter_enable (uint8_t id, uint8_t prescaler)
{
  if (id > MAX_ID)
    return;

  register_set (id, CONTROL_REGISTER_B, prescaler & PRESCALER_MASK);
}

void counter_disable (uint8_t id)
{
  if (id > MAX_ID)
    return;

  register_clear (id, CONTROL_REGISTER_B, PRESCALER_MASK);
}

void counter_interrupt (uint8_t enable, uint8_t counter_id, counter_handle fun)
{
  if ((counter_id > MAX_ID)
      || ((enable != 0)
          && (fun == 0)))
    return;

  uint8_t flag = (eight_bits (counter_id) != 0)
    ? FLAG_ENABLE_COMPARE_A_8 : FLAG_ENABLE_COMPARE_A_16;
  uint8_t control_reg = (eight_bits (counter_id) != 0)
    ? CONTROL_REGISTER_A : CONTROL_REGISTER_B;

  if (enable != 0) {
    compare_a[counter_id] = fun;
    register_set (counter_id, control_reg, flag);
    register_set (counter_id,
                  CONTROL_REGISTER_INTERRUPT, FLAG_INTERRUPT_COMPARE_A);
  } else {
    register_clear (counter_id, control_reg, flag);
    register_clear (counter_id,
                    CONTROL_REGISTER_INTERRUPT, FLAG_INTERRUPT_COMPARE_A);
  }
}

void counter_set_register (uint8_t id, uint8_t reg, uint8_t low, uint8_t high)
{
  volatile uint8_t *reg_array[] =
    {
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
  const uint8_t factor = (reg == COUNTER_OUTPUT_COMPARE_B) ? 1 : 0;
  const uint8_t reg_array_id = id + factor * shift;

  volatile uint8_t *lhs_low = reg_array[reg_array_id];
  volatile uint8_t *lhs_high = reg_array[reg_array_id + 1];

  *lhs_low = low;

  if (eight_bits (id) == 0)
    *lhs_high = high;
}

static void interrupt_routine (uint8_t counter_id)
{
  buf_byte_fill (&handle_queue, counter_id);
}

ISR(TIMER0_COMPA_vect)
{
  interrupt_routine (COUNTER_0);
}

ISR(TIMER1_COMPA_vect)
{
  interrupt_routine (COUNTER_1);
}

ISR(TIMER2_COMPA_vect)
{
  interrupt_routine (COUNTER_2);
}

ISR(TIMER3_COMPA_vect)
{
  interrupt_routine (COUNTER_3);
}

ISR(TIMER4_COMPA_vect)
{
  interrupt_routine (COUNTER_4);
}

ISR(TIMER5_COMPA_vect)
{
  interrupt_routine (COUNTER_5);
}

void counter_try ()
{
  uint8_t id = 0;
  if ((buf_byte_drain (&handle_queue, &id) == 0)
      || (id > MAX_ID))
    return;

  /* if (fun != 0) --- it doesn't work ---*/
  compare_a[id] ();
}
