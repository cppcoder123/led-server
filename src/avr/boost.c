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
#define BOOST_COMPARE_A 160
#define BOOST_PWM_MIN 3
#define BOOST_PWM_MAX 157
#define BOOST_PWM_MIDDLE ((BOOST_PWM_MIN + BOOST_PWM_MAX) / 2)
#define BOOST_COMPARE_A_HIGH 0

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
  counter_enable (BOOST_COUNTER, BOOST_PRESCALER);
  counter_set_register (BOOST_COUNTER, COUNTER_OUTPUT_COMPARE_A,
                        BOOST_PWM_MIDDLE, BOOST_COMPARE_A_HIGH);
 /* fixme : set proper flags */
}

static void counter_stop ()
{
  /* fixme */
  counter_disable (BOOST_COUNTER);
  /* fixme */
  /* counter_disable_compare_a (); */
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
  counter_stop ();
  adc_stop ();
}

void boost_try ()
{
  /* fixme */
  feedback_try (&feedback);
}
