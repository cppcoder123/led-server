/*
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "unix/constant.h"

#include "boost.h"
#include "counter.h"
#include "debug.h"
#include "feedback.h"

/* 12 volt, fixme check */
#define FEEDBACK_TARGET 102
/* suitable delta ? 10% fixme */
#define FEEDBACK_DELTA 2
/* delay , 5 times? fixme */
#define FEEDBACK_DELAY 2
/* ignore first measurements */
#define FEEDBACK_IGNORE 100

#define BOOST_COUNTER COUNTER_4
#define BOOST_PRESCALER COUNTER_PRESCALER_1
/* ~25 kHz */
#define BOOST_FREQUENCY 160

/* min - idle run? */
#define PWM_MIN 0
#define PWM_MAX BOOST_FREQUENCY
#define PWM_DELTA_FINE 1
#define PWM_DELTA_ROUGH 1

/* tiny - delta is very small, no need to tune */
#define VOLTAGE_DELTA_TINY FEEDBACK_DELTA
/* fine - carefull tuning is needed, more than this is rough */
#define VOLTAGE_DELTA_FINE (5 * FEEDBACK_DELTA)

#define BOOST_ZERO 0

#define BOOST_PORT PORTH4

static struct feedback_t feedback;
static uint8_t pwm = 0;

static void adc_start ()
{
  /* fixme */

  /* 0 pin is connected to adc => no mux adjustment */
  /* 1 for ADLAR => left adjust result, no accuracy*/
  /* AVCC is for ref, -2.56V internal ref- */
  ADMUX = 0;
  /* ADMUX |= (1 < REFS0) | (1 << ADLAR); */
  /* ADMUX |= (1 << REFS1) | (1 < REFS0) | (1 << ADLAR); */
  ADMUX |= (1 << REFS1) | (1 << ADLAR);

  /*free running mode => no ADCSRB adjustment 000 for ADTS*/

  /* disable digital pin for adc0 */
  DIDR0 |= (1 << ADC0D);

  /*enable adc, start conversion, enable interrupt, */
  /* auto triggering, 128 clock division factor*/
  ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE)
    | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

static void adc_stop ()
{
  /* fixme */
  ADCSRA &= ~((1 << ADEN) | (1 << ADIE));
}

ISR(ADC_vect)
{
  /* ADMUX &= 0xF0; */
  const uint8_t data = ADCH;
  feedback_data (&feedback, data/*ADCH*/);
  /* debug_1 (DEBUG_BOOST, 55, data); */
  /* ADCSRA |= (1 << ADSC); */
}

static void counter_start ()
{
  /* fixme */
  counter_register_write
    (BOOST_COUNTER, COUNTER_OUTPUT_COMPARE_A, BOOST_FREQUENCY, BOOST_ZERO);
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
  if (param_delta <= VOLTAGE_DELTA_TINY)
    return BOOST_ZERO;

  if (param_delta <= VOLTAGE_DELTA_FINE)
    return PWM_DELTA_FINE;

  /* delta is very big */
  return PWM_DELTA_ROUGH;
}

static void control (uint8_t current)
{
  /* fixme */
  uint8_t need_more = (current < FEEDBACK_TARGET) ? 1 : 0;
  uint8_t param_delta = (need_more > 0)
    ? FEEDBACK_TARGET - current : current - FEEDBACK_TARGET;
  uint8_t pwm_delta = get_pwm_delta (param_delta);

  if (pwm_delta == BOOST_ZERO) {
    /* we shouldn't be here */
    debug_1 (DEBUG_BOOST, 99, param_delta);
    return;
  }

  if (need_more > 0)
    pwm = (pwm + pwm_delta < PWM_MAX)
      ? pwm + pwm_delta : PWM_MAX;
  else
    pwm = ((pwm > pwm_delta) && (pwm - pwm_delta > PWM_MIN))
           ? pwm - pwm_delta : PWM_MIN;

  debug_1 (DEBUG_BOOST, 88, pwm);

  counter_register_write (BOOST_COUNTER,
                          COUNTER_OUTPUT_COMPARE_B, pwm, BOOST_ZERO);
}

void boost_start ()
{
  /* init sw part */
  feedback_init (&feedback, FEEDBACK_TARGET, FEEDBACK_DELTA,
                 FEEDBACK_DELAY, FEEDBACK_IGNORE, &control);
  pwm = PWM_MIN;
  /* init hw part */
  /* enable boost port */
  DDRH |= (1 << BOOST_PORT);
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
