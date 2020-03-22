/*
 *
 */

#include <stdint.h>

#include "unix/constant.h"

#include "boost.h"
#include "counter.h"
#include "cron.h"
#include "debug.h"
#include "feedback.h"
#include "fan.h"

/* fixme, needed rev/min */
#define FEEDBACK_TARGET 50
/* fixme, needed parameter accuracy, 10% ? */
#define FEEDBACK_DELTA 5
/* fixme, how responsive is feedback */
#define FEEDBACK_DELAY 10
/* ignore first measurements */
#define FEEDBACK_IGNORE 100

#define PWM_COUNTER COUNTER_5
#define PWM_PRESCALER COUNTER_PRESCALER_1
#define PWM_FREQUENCY 160
#define PWM_MAX PWM_FREQUENCY
#define PWM_MIN 2
#define PWM_DELTA_FINE 1
#define PWM_DELTA_ROUGH 5

#define METER_COUNTER COUNTER_1
#define METER_PRESCALER COUNTER_PRESCALER_1
/* measure frequency, 1 time per second, fixme?*/
#define METER_FREQUENCY 50
/* wait 10 secs before starting measurements */
/* #define METER_DELAY 10 */
/* difference is too small => no correction is needed */
#define METER_DELTA_TINY FEEDBACK_DELTA
/* diff is small, correct carefully*/
#define METER_DELTA_FINE (5 * FEEDBACK_DELTA)

#define FAN_ZERO 0

static struct feedback_t feedback;
static uint8_t started = 0;
static uint8_t pwm = PWM_MAX;
/* give fan a time to gain momentum */
static uint8_t prepare = 1;

void fan_init ()
{
  /* fixme */
  started = 0;
}

void fan_try ()
{
  /* fixme */
  if (started == 0)
    return;

  boost_try ();
  feedback_try (&feedback);
}

static void start_pwm ()
{
  pwm = PWM_MAX;
  counter_register_write
    (PWM_COUNTER, COUNTER_OUTPUT_COMPARE_A, PWM_FREQUENCY, FAN_ZERO);
  counter_register_write
    (PWM_COUNTER, COUNTER_OUTPUT_COMPARE_B, pwm, FAN_ZERO);
  counter_pwm (1, PWM_COUNTER);
  counter_enable (PWM_COUNTER, PWM_PRESCALER);
}

static void stop_pwm ()
{
  counter_disable (PWM_COUNTER);
  counter_pwm (0, PWM_COUNTER);
}

static void measure ()
{
  if (prepare > 0) {
    /* prepare for measurements */
    prepare = 0;
    counter_register_write (METER_COUNTER, COUNTER_VALUE, FAN_ZERO, FAN_ZERO);
    return;
  }

  uint8_t low, high;
  counter_register_read (METER_COUNTER, COUNTER_VALUE, &low, &high);
  counter_register_write (METER_COUNTER, COUNTER_VALUE, FAN_ZERO, FAN_ZERO);

  feedback_data (&feedback, low);

  if (high != FAN_ZERO)
    debug_1 (DEBUG_FAN, 222, high);
}

static void start_meter ()      /* frequency-meter */
{
  prepare = 1;
  counter_register_write (METER_COUNTER, COUNTER_VALUE, FAN_ZERO, FAN_ZERO);
  counter_enable (METER_COUNTER, METER_PRESCALER);
  cron_enable (CRON_ID_FAN, METER_FREQUENCY, &measure);
}

static void stop_meter ()
{
  cron_disable (CRON_ID_FAN);
  counter_disable (METER_COUNTER);
}

static uint8_t get_pwm_delta (uint8_t meter_delta)
{
  if (meter_delta <= METER_DELTA_TINY)
    return FAN_ZERO;
  if (meter_delta <= METER_DELTA_FINE)
    return PWM_DELTA_FINE;

  return PWM_DELTA_ROUGH;
}

static void control (uint8_t current)
{
  /* fixme */
  uint8_t need_more = (current < FEEDBACK_TARGET) ? 1 : 0;
  uint8_t meter_delta = (need_more > 0)
    ? FEEDBACK_TARGET - current : current - FEEDBACK_TARGET;
  uint8_t pwm_delta = get_pwm_delta (meter_delta);

  if (pwm_delta == FAN_ZERO) {
    debug_1 (DEBUG_FAN, 111, meter_delta);
    return;
  }

  if (need_more > 0)
    pwm = (pwm + pwm_delta < PWM_MAX) ? pwm + pwm_delta : PWM_MAX;
  else
    pwm = ((pwm > pwm_delta) && (pwm - pwm_delta > PWM_MIN))
      ? pwm - pwm_delta : PWM_MIN;

  counter_register_write (PWM_COUNTER,
                          COUNTER_OUTPUT_COMPARE_B, pwm, FAN_ZERO);
}

void fan_start ()
{
  started = 1;

  boost_start ();
  /* debug */
  return;
  feedback_init (&feedback, FEEDBACK_TARGET, FEEDBACK_DELTA,
                 FEEDBACK_DELAY, FEEDBACK_IGNORE, &control);
  start_pwm ();
  start_meter ();
  /* fixme */
}

void fan_stop ()
{
  /* fixme */
  stop_meter ();
  stop_pwm ();
  boost_stop ();

  started = 0;
}
