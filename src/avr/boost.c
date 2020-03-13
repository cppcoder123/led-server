/*
 *
 */

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

static struct feedback_t feedback;

static void adc_start ()
{
  /* fixme */
}

static void adc_stop ()
{
  /* fixme */
}

static void counter_start ()
{
  /* fixme */
  counter_register_write
    (BOOST_COUNTER, COUNTER_OUTPUT_COMPARE_A, BOOST_COMPARE_A_LOW, BOOST_ZERO);
  counter_register_write
    (BOOST_COUNTER, COUNTER_OUTPUT_COMPARE_B, BOOST_PWM_MIN, BOOST_ZERO);
  counter_pwm (1, BOOST_COUNTER);
  counter_enable (BOOST_COUNTER, BOOST_PRESCALER);
}

static void counter_stop ()
{
  /* fixme */
  counter_disable (BOOST_COUNTER);
  counter_pwm (0, BOOST_COUNTER);
}

static void control (uint8_t current)
{
  /* fixme */
}

void boost_start ()
{
  /* init sw part */
  feedback_init (&feedback, TARGET, DELTA, DELAY, &control);
  /* init hw part, fixme */
  counter_start ();
  adc_start ();
}

void boost_stop ()
{
  /* uninit hw part, fixme */
  adc_stop ();
  counter_stop ();
}

void boost_try ()
{
  /* fixme */
  feedback_try (&feedback);
}
