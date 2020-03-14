/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "boost.h"
#include "counter.h"
#include "feedback.h"

/* 12 volt, fixme check */
#define TARGET 125

/* suitable delta ? 10% fixme */
#define DELTA 12

/* delay , 5 times? fixme */
#define DELAY 5

#define BOOST_COUNTER COUNTER_4
#define BOOST_PRESCALER COUNTER_PRESCALER_1
/* 25 kHz */
#define BOOST_COMPARE_A_LOW 160
#define BOOST_ZERO 0
/* */
#define BOOST_PWM_MIN 4
#define BOOST_PWM_MAX 157

/* handle pwm feedback */
/* epsilon - delta is very small, no need to tune */
#define PARAM_DELTA_EPSILON 3
/* fine - carefull tuning is needed, more than this is rough */
#define PARAM_DELTA_FINE 10
/* ---------- */
#define PWM_DELTA_FINE 1
#define PWM_DELTA_ROUGH 5

static struct feedback_t feedback;
static uint8_t pwm = 0;

static void adc_start ()
{
  /* fixme */

  /* 0 pin is connected to adc => no mux adjustment */
  /* 1 for ADLAR => left adjust result, no accuracy*/
  /* AVCC with external capacitor at AREF */
  ADMUX |= (1 < REFS0) | (1 << ADLAR);

  /*free running mode => no ADCSRB adjustment */

  /* disable digital pin for adc0 */
  DIDR0 |= (1 << ADC0D);

  /*enable adc, enable interrupt, auto triggering, 128 clock division factor*/
  ADCSRA |= (1 << ADEN) | (1 << ADATE)
    | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

static void adc_stop ()
{
  /* fixme */
  ADCSRA &= ~((1 << ADEN) | (1 << ADIE));
}

ISR(ADC_vect)
{
  feedback_data (&feedback, ADCH);
}

static void counter_start ()
{
  /* fixme */
  counter_register_write
    (BOOST_COUNTER, COUNTER_OUTPUT_COMPARE_A, BOOST_COMPARE_A_LOW, BOOST_ZERO);
  counter_register_write
    (BOOST_COUNTER, COUNTER_OUTPUT_COMPARE_B, pwm, BOOST_ZERO);
  counter_pwm (1, BOOST_COUNTER);
  counter_enable (BOOST_COUNTER, BOOST_PRESCALER);
}

static void counter_stop ()
{
  /* fixme */
  counter_disable (BOOST_COUNTER);
  counter_pwm (0, BOOST_COUNTER);
}

static uint8_t get_pwm_delta (uint8_t param_delta)
{
  if ((param_delta == 0)
      || (param_delta <= PARAM_DELTA_EPSILON))
    return 0;

  if (param_delta <= PARAM_DELTA_FINE)
    return PWM_DELTA_FINE;

  /* delta is very big */
  return PWM_DELTA_ROUGH;
}

static void control (uint8_t current)
{
  /* fixme */
  uint8_t need_more = (current < TARGET) ? 1 : 0;
  uint8_t param_delta = (need_more > 0) ? TARGET - current : current - TARGET;
  uint8_t pwm_delta = get_pwm_delta (param_delta);

  if (need_more > 0)
    pwm = (pwm + pwm_delta < BOOST_PWM_MAX)
      ? pwm + pwm_delta : BOOST_PWM_MAX;
  else
    pwm = ((pwm > pwm_delta) && (pwm - pwm_delta > BOOST_PWM_MIN))
           ? pwm - pwm_delta : BOOST_PWM_MIN;

  counter_register_write (BOOST_COUNTER,
                          COUNTER_OUTPUT_COMPARE_B, pwm, BOOST_ZERO);
}

void boost_start ()
{
  /* init sw part */
  feedback_init (&feedback, TARGET, DELTA, DELAY, &control);
  pwm = BOOST_PWM_MIN;
  /* init hw part */
  counter_start ();
  adc_start ();
}

void boost_stop ()
{
  /* uninit hw part */
  adc_stop ();
  counter_stop ();
}

void boost_try ()
{
  /* feedback_try calls 'control' if needed */
  feedback_try (&feedback);
}
