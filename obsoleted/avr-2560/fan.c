/*
 *
 */

#include <avr/io.h>

#include <stdint.h>

#include "unix/constant.h"

#include "at.h"
#include "counter.h"
#include "cron.h"
#include "debug.h"
#include "feedback.h"
#include "fan.h"

/* fixme, needed rev/min */
#define FEEDBACK_TARGET 30
/* fixme, needed parameter accuracy, 10% ? */
#define FEEDBACK_DELTA 2
/* fixme, how responsive is feedback */
/* #define FEEDBACK_DELAY 2 */
/* ignore first measurements */
#define FEEDBACK_IGNORE 3

#define PWM_COUNTER COUNTER_3
#define PWM_PRESCALER COUNTER_PRESCALER_1
#define PWM_FREQUENCY 160
#define PWM_MAX PWM_FREQUENCY
#define PWM_MIN 2
/* #define PWM_START ((PWM_MAX + PWM_MIN) / 2)*/
#define PWM_START PWM_MIN
#define PWM_DELTA_FINE 1
#define PWM_DELTA_ROUGH 5

#define METER_COUNTER COUNTER_5
#define METER_PRESCALER COUNTER_PRESCALER_EXT_RISE
/* measure frequency, once per 5 sec?*/
#define METER_FREQUENCY 250
/* difference is too small => no correction is needed */
#define METER_DELTA_TINY FEEDBACK_DELTA
/* diff is small, correct carefully*/
#define METER_DELTA_FINE (3 * FEEDBACK_DELTA)

#define FAN_ZERO 0

#define PWM_BIT PORTE4
/* #define PWM_PORT PORTE */
#define PWM_DDR DDRE
/* negative polarity? it needs to be checked */
#define PWM_POLARITY 0

#define POWER_BIT PORTH1
#define POWER_PORT PORTH
#define POWER_DDR DDRH

#define ON 1
#define OFF 0

#define FAN_INIT_DURATION 255

static struct feedback_t feedback;
static uint8_t started = 0;
static uint8_t pwm_value = PWM_MAX;

void fan_try ()
{
  /* fixme */
  if (started == 0)
    return;

  feedback_try (&feedback);
}

static void power (uint8_t arg)
{
  if (arg == ON) {
    /* configure power wire as output */
    POWER_DDR |= (1 << POWER_BIT);
    /* turn on power, assign 0 */
    POWER_PORT &= ~(1 << POWER_BIT);
  } else {                      /* off */
    /* turn off power, assign 1 */
    POWER_PORT |= (1 << POWER_BIT);
    /* configure power wire as input */
    POWER_DDR &= ~(1 << POWER_BIT);
  }
}

static void pwm (uint8_t arg)
{
  if (arg == ON) {
    /* configure as output */
    PWM_DDR |= (1 << PWM_BIT);

    pwm_value = PWM_START;
    counter_register_write
      (PWM_COUNTER, COUNTER_OUTPUT_COMPARE_A, PWM_FREQUENCY, FAN_ZERO);
    counter_register_write
      (PWM_COUNTER, COUNTER_OUTPUT_COMPARE_B, pwm_value, FAN_ZERO);
    counter_pwm_enable (PWM_COUNTER, PWM_POLARITY);
    counter_enable (PWM_COUNTER, PWM_PRESCALER);
  } else {                      /* off */
    counter_disable (PWM_COUNTER);
    counter_pwm_disable (PWM_COUNTER, PWM_POLARITY);
    /* release pwm wire */
    PWM_DDR &= ~(1 << PWM_BIT);
  }
}

static void measure ()
{
  uint8_t low = 0, high = 0;
  counter_register_read (METER_COUNTER, COUNTER_VALUE, &low, &high);
  counter_register_write (METER_COUNTER, COUNTER_VALUE, FAN_ZERO, FAN_ZERO);

  if ((low > 0x7F) && (high < 0xFF))
    ++high;

  feedback_data (&feedback, high);

  /* if (high != FAN_ZERO) */
  /* debug_1 (DEBUG_FAN, 222, high); */
}

static void meter (uint8_t arg) /* frequency-meter */
{
  if (arg == ON) {
    counter_register_write (METER_COUNTER, COUNTER_VALUE, FAN_ZERO, FAN_ZERO);
    counter_enable (METER_COUNTER, METER_PRESCALER);
    cron_enable (CRON_ID_FAN, METER_FREQUENCY, &measure);
  } else {                      /* off */
    cron_disable (CRON_ID_FAN);
    counter_disable (METER_COUNTER);
  }
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

  pwm_value = (need_more > 0)
    ? ((pwm_value + pwm_delta < PWM_MAX) ? pwm_value + pwm_delta : PWM_MAX)
    : (((pwm_value > pwm_delta) && (pwm_value - pwm_delta > PWM_MIN))
       ? pwm_value - pwm_delta : PWM_MIN);

  /* debug_1 (DEBUG_FAN, 123, pwm); */

  /* fixme */
  /* pwm = 160; */

  counter_register_write (PWM_COUNTER,
                          COUNTER_OUTPUT_COMPARE_B, pwm_value, FAN_ZERO);
}

void fan_start ()
{
  started = 1;

  feedback_init (&feedback, FEEDBACK_TARGET, FEEDBACK_DELTA,
                 /* FEEDBACK_DELAY, */ FEEDBACK_IGNORE, &control);
  power (ON);
  pwm (ON);
  meter (ON);
  /* fixme */
}

void fan_stop ()
{
  /* fixme */
  meter (OFF);
  pwm (OFF);
  power (OFF);

  started = 0;
}

void fan_init ()
{
  /* fixme */
  started = 0;

  fan_start ();
  at_schedule (AT_FAN, FAN_INIT_DURATION, &fan_stop);
}
