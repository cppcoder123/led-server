/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "buf.h"
#include "counter.h"
#include "debug.h"

#define MAX_ID COUNTER_5

#define PRESCALER_MASK \
  (COUNTER_PRESCALER_1 | COUNTER_PRESCALER_8 | COUNTER_PRESCALER_256)

/*
 * enable CTC
 * ----------
 *
 * WGM01, WGM21
 */
#define FLAG_8_ENABLE_CTC_COMPARE_A (1 << 1)
/*
 * WGM12, WGM32, WGM42, WGM52
 */
#define FLAG_16_ENABLE_CTC_COMPARE_A (1 << 3)

/*
 * Define Fast PWM with OCRA and switch OCRB output
 *
 *
 * 8 bit counters
 * --------------
 *
 *
 * Register A:
 *
 * (1 << 0) - WGMx0
 * (1 << 1) - WGMx1
 *
 * (1 << 4) - COMxB0
 * (1 << 5) - COMxB1
 */
#define FLAG_8_ENABLE_PWM_A ((1 << 0) | (1 << 1) | (1 << 5))
#define FLAG_8_ENABLE_PWM_A_INVERT (1 << 4)
/*
 * Register B:
 *
 * (1 << 3) - WGMx2
 */
#define FLAG_8_ENABLE_PWM_B (1 << 3)

/*
 * 16 bit counters
 * ---------------
 *
 * (1 << 0) - WGMx0 one of fast pwm flags
 * (1 << 1) - WGMx1
 * (1 << 5) - is non inverting mode,
 * (1 << 4) - together with (1 << 5) inverting mode
 */
#define FLAG_16_ENABLE_PWM_A ((1 << 0) | (1 << 1) | (1 << 5))
#define FLAG_16_ENABLE_PWM_A_INVERT (1 << 4)
/*
 * (1 << 3) - WGMx2
 * (1 << 4) - WGMx3
 */
#define FLAG_16_ENABLE_PWM_B ((1 << 3) | (1 << 4))

/*
 * enable interrupt
 * ----------------
 */
#define FLAG_INTERRUPT_COMPARE_A (1 << 1)

enum {
  CONTROL_REGISTER_A,
  CONTROL_REGISTER_B,
  CONTROL_REGISTER_INTERRUPT,
};

typedef void (*handle_flag) (uint8_t counter_id, uint8_t reg_id, uint8_t flag);

static counter_callback callback_array[MAX_ID + 1];

static struct buf_t invoke_queue;

void counter_init ()
{
  for (uint8_t i = 0; i <= MAX_ID; ++i)
    callback_array[i] = 0;

  buf_init (&invoke_queue);
}

static uint8_t eight_bits (uint8_t id)
{
  return ((id == COUNTER_0) || (id == COUNTER_2)) ? 1 : 0;
}

static volatile uint8_t* control_register_get (uint8_t counter_id,
                                               uint8_t register_id)
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

static void control_register_set (uint8_t counter_id,
                                  uint8_t register_id, uint8_t value)
{
  volatile uint8_t *reg = control_register_get (counter_id, register_id);
  *reg |= value;
}

static void control_register_clear (uint8_t counter_id,
                                    uint8_t register_id, uint8_t value)
{
  volatile uint8_t *reg = control_register_get (counter_id, register_id);
  *reg &= ~value;
}

void counter_enable (uint8_t id, uint8_t prescaler)
{
  if (id > MAX_ID)
    return;

  control_register_set (id, CONTROL_REGISTER_B, prescaler & PRESCALER_MASK);
}

void counter_disable (uint8_t id)
{
  if (id > MAX_ID)
    return;

  control_register_clear (id, CONTROL_REGISTER_B, PRESCALER_MASK);
}

static void counter_interrupt (uint8_t enable,
                               uint8_t counter_id, counter_callback fun)
{
  if ((counter_id > MAX_ID)
      || ((enable != 0)
          && (fun == 0)))
    return;

  uint8_t flag = (eight_bits (counter_id) != 0)
    ? FLAG_8_ENABLE_CTC_COMPARE_A : FLAG_16_ENABLE_CTC_COMPARE_A;
  uint8_t control_reg = (eight_bits (counter_id) != 0)
    ? CONTROL_REGISTER_A : CONTROL_REGISTER_B;
  handle_flag handle = (enable != 0)
    ? &control_register_set : &control_register_clear;

  if (enable != 0)
    callback_array[counter_id] = fun;

  handle (counter_id, control_reg, flag);
  handle (counter_id, CONTROL_REGISTER_INTERRUPT, FLAG_INTERRUPT_COMPARE_A);
}

void counter_interrupt_enable (uint8_t counter_id, counter_callback fun)
{
  counter_interrupt (1, counter_id, fun);
}

void counter_interrupt_disable (uint8_t counter_id)
{
  counter_interrupt (0, counter_id, 0);
}

static void counter_pwm (uint8_t enable, uint8_t counter_id, uint8_t positive)
{
  handle_flag handle = (enable != 0)
    ? &control_register_set : &control_register_clear;

  const uint8_t eight = eight_bits (counter_id);

  const uint8_t invert = (positive > 0) ? 0
    : (eight != 0) ? FLAG_8_ENABLE_PWM_A_INVERT : FLAG_16_ENABLE_PWM_A_INVERT;

  const uint8_t flag_a
    = ((eight != 0) ? FLAG_8_ENABLE_PWM_A : FLAG_16_ENABLE_PWM_A) | invert;

  const uint8_t flag_b = (eight != 0)
    ? FLAG_8_ENABLE_PWM_B : FLAG_16_ENABLE_PWM_B;

  handle (counter_id, CONTROL_REGISTER_A, flag_a);
  handle (counter_id, CONTROL_REGISTER_B, flag_b);
}

void counter_pwm_enable (uint8_t counter_id, uint8_t positive)
{
  counter_pwm (1, counter_id, positive);
}

void counter_pwm_disable (uint8_t counter_id, uint8_t positive)
{
  counter_pwm (0, counter_id, positive);
}

static void value_register_get (uint8_t counter_id, uint8_t reg_id,
                                volatile uint8_t **reg_low, volatile uint8_t **reg_high)
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
     &OCR5BL, &OCR5BH,
     &TCNT0, &TCNT0,            /* counter itself */
     &TCNT1L, &TCNT1H,
     &TCNT2, &TCNT2,
     &TCNT3L, &TCNT3H,
     &TCNT4L, &TCNT4H,
     &TCNT5L, &TCNT5H,
  };

  const uint8_t shift = (reg_id == COUNTER_OUTPUT_COMPARE_B) ? 12
    : (reg_id == COUNTER_VALUE) ? 24 : 0;

  const uint8_t reg_array_id = counter_id * 2 + shift;

  *reg_low = reg_array[reg_array_id];
  if (eight_bits (counter_id) == 0)
    *reg_high = reg_array[reg_array_id + 1];
}

void counter_register_write (uint8_t counter_id,
                             uint8_t reg_id, uint8_t low, uint8_t high)
{
  volatile uint8_t *reg_low = 0;
  volatile uint8_t *reg_high = 0;
  value_register_get (counter_id, reg_id, &reg_low, &reg_high);

  /* ! high byte first */
  if (eight_bits (counter_id) == 0)
    *reg_high = high;
  *reg_low = low;
}

void counter_register_read (uint8_t counter_id,
                            uint8_t reg_id, uint8_t *low, uint8_t *high)
{
  volatile uint8_t *reg_low = 0;
  volatile uint8_t *reg_high = 0;
  value_register_get (counter_id, reg_id, &reg_low, &reg_high);

  /* ! low byte first */
  *low = *reg_low;
  if (eight_bits (counter_id) == 0)
    *high = *reg_high;
}

ISR(TIMER0_COMPA_vect)
{
  buf_byte_fill (&invoke_queue, COUNTER_0);
}

ISR(TIMER1_COMPA_vect)
{
  buf_byte_fill (&invoke_queue, COUNTER_1);
}

ISR(TIMER2_COMPA_vect)
{
  buf_byte_fill (&invoke_queue, COUNTER_2);
}

ISR(TIMER3_COMPA_vect)
{
  buf_byte_fill (&invoke_queue, COUNTER_3);
}

ISR(TIMER4_COMPA_vect)
{
  buf_byte_fill (&invoke_queue, COUNTER_4);
}

ISR(TIMER5_COMPA_vect)
{
  buf_byte_fill (&invoke_queue, COUNTER_5);
}

void counter_try ()
{
  uint8_t id = 0;
  if ((buf_byte_drain (&invoke_queue, &id) == 0)
      || (id > MAX_ID))
    return;

  /* if (fun != 0) --- it doesn't work ---*/
  callback_array[id] ();
}
