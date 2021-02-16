/*
 *
 */
#include <avr/interrupt.h>
#include <avr/io.h>

#include "at.h"
#include "buf.h"
#include "buzz.h"
#include "debug.h"
#include "flush.h"
#include "font.h"
#include "render.h"
#include "twi.h"
#include "watch.h"

#define RTC_ADDRESS 0x68

#define REG_CONTROL 0x0e
#define REG_ENABLE REG_CONTROL

#define REG_STATUS 0x0f
#define REG_ENABLE_32KHZ REG_STATUS

#define REG_SECOND 0
#define REG_MINUTE 1
#define REG_HOUR 2

/*
 *     Bit: 7     6     5    4   3   2     1    0
 *          !EOSC BBSQW CONV RS2 RS1 INTCN A2IE A1IE
 * enable:  0     0     0    0   0   0     0    0
 * disable: 0     0     0    0   0   1     0    0
 *
 * RS2 == RS1 == 0 => 1Hz
 * INTCN 0 enables SQW pulse signal
 */
#define REG_VALUE_ENABLE 0
#define REG_VALUE_DISABLE 4

/*
 * It looks 32kHz signal is enabled by default,
 * Do we need to disable it?
 *
 *  Status register:
 *
 *     Bit: 7   6 5 4 3     2    1   0
 *          OSF R R R 32kHz Busy A2F A1F
 * enable:  0   0 0 0 1     0    0   0
 * disable: 0   0 0 0 0     0    0   0
 *
 *   => 0 disables 32kHz
 */
#define REG_VALUE_DISABLE_32KHZ 0

/* set initial time */
#define INITIAL_TIME 1

/* 10 pixels from left */
#define IMAGE_INDENT 10

/*
 * Buffer, only for read
 */
enum {
      BUFFER_SECOND,
      BUFFER_MINUTE,
      BUFFER_HOUR,
      BUFFER_SIZE               /* ! Keep it last */
};

static uint8_t buffer[BUFFER_SIZE];

/*
 * Event: just tags for callbacks
 */
enum {
      TAG_ENABLE,
      TAG_DISABLE,
      TAG_DISABLE_32KHZ,
      TAG_HOUR,
      TAG_MINUTE,
      TAG_SECOND,
};

enum {
      RTC_TO,                   /* convert time to ds3231 format */
      RTC_FROM,                 /* from */
      RTC_HOUR,
      RTC_MINUTE,
      RTC_SECOND,
};

/*
 * Alarm related
 */
#define ALARM_DURATION 10

static uint8_t alarm_engaged = 0;
static uint8_t alarm_hour = 0;
static uint8_t alarm_minute = 0;

/*
 * Convert hour/minute/second to/from ds3231 format.
 *
 * hours:
 *    Assume 24h mode:
 *    Right for bits are hours (less than 10),
 *    bits 5 and 6 represent 10-nth of hours
 *
 * minutes/seconds:
 *    Right 4 bits are minutes (less than 10),
 *    bits 5,6,7 are tens of seconds
 */
static uint8_t rtc (uint8_t src, uint8_t direction, uint8_t unit)
{
  uint8_t result = 0;

  if (direction == RTC_TO) {
    if ((unit == RTC_HOUR)
        && (src > 23))
      src = 23;
    if ((unit != RTC_HOUR)
        && (src > 59))
      src = 59;
    result = ((src / 10) << 4) | (src % 10);
  } else {                      /* from rtc */
    const uint8_t tens_mask = (unit == RTC_HOUR) ? 0x3 : 0x7;
    result = ((src >> 4) & tens_mask) * 10 + (src & 0xf);
  }

  return result;
}

uint8_t watch_alarm_set (uint8_t hour, uint8_t minute)
{
  if ((hour > WATCH_HOUR_MAX)
      || (minute > WATCH_MINUTE_MAX))
    return 0;

  alarm_hour = hour;
  alarm_minute = minute;

  return 1;
}

void watch_alarm_get (uint8_t *hour, uint8_t *minute)
{
  *hour = alarm_hour;
  *minute = alarm_minute;
}

uint8_t watch_alarm_state ()
{
  return alarm_engaged;
}

/* 0 disables the alarm */
void watch_alarm_control (uint8_t engage)
{
  alarm_engaged = (engage == 0) ? 0 : 1;
}

static void alarm_check (uint8_t hour, uint8_t minute)
{
  if ((alarm_hour != hour) || (alarm_minute != minute))
    return;

  buzz_start ();
  at_schedule (AT_WATCH, ALARM_DURATION, &buzz_stop);
}

static void render () /* send watch value into display */
{
  struct buf_t image;
  buf_init (&image);

  for (uint8_t i = 0; i < IMAGE_INDENT; ++i)
    buf_byte_fill (&image, 0);

  uint8_t time_buf = rtc (buffer[BUFFER_HOUR], RTC_FROM, RTC_HOUR);
  render_number (&image, time_buf, RENDER_LEADING_DISABLE);
  render_symbol (&image, FONT_COLON);

  time_buf = rtc (buffer[BUFFER_MINUTE], RTC_FROM, RTC_MINUTE);
  render_number (&image, time_buf, RENDER_LEADING_TEN);
  render_symbol (&image, FONT_COLON);

  time_buf = rtc (buffer[BUFFER_SECOND], RTC_FROM, RTC_SECOND);
  render_number (&image, time_buf, RENDER_LEADING_TEN);

  render_tail (&image);

  flush_stable_display (&image);
}

static void read_callback (uint8_t tag, uint8_t status, uint8_t len,
                           volatile uint8_t *value)
{
  if ((status != TWI_SUCCESS)
      || (tag >= BUFFER_SIZE)
      || (len < 1))
    return;

  buffer[tag] = value[0];

  if ((tag == BUFFER_SECOND)
      && (buffer[BUFFER_SECOND] == 0)
      && (watch_alarm_state ()))
    alarm_check (buffer[BUFFER_HOUR], buffer[BUFFER_MINUTE]);

  render ();
}

void watch_enable ()
{
  twi_write_byte (TWI_ID_RTC, TAG_ENABLE, REG_ENABLE, REG_VALUE_ENABLE);
}

void watch_disable ()
{
  twi_write_byte (TWI_ID_RTC, TAG_DISABLE, REG_ENABLE, REG_VALUE_DISABLE);
}

void watch_set (uint8_t hour, uint8_t minute, uint8_t second)
{
  twi_write_byte (TWI_ID_RTC, TAG_HOUR,
                  REG_HOUR, rtc (hour, RTC_TO, RTC_HOUR));
  twi_write_byte (TWI_ID_RTC, TAG_MINUTE,
                  REG_MINUTE, rtc (minute, RTC_TO, RTC_MINUTE));
  twi_write_byte (TWI_ID_RTC, TAG_SECOND,
                  REG_SECOND, rtc (second, RTC_TO, RTC_SECOND));
}

void watch_init ()
{
  /*
   * Register for 1Hz signal
   *
   * 1 & 1 => rising edge
   */
  EICRA |= ((1 << ISC21) | (1 << ISC20));
  /*
   * Enable INT2 interrupt
   */
  EIMSK |= (1 << INT2);

  for (uint8_t i = 0; i < BUFFER_SIZE; ++i)
    buffer[i] = 0;

  watch_alarm_set (0, 0);
  watch_alarm_control (0);      /* disengage */

  twi_slave_r (TWI_ID_RTC, RTC_ADDRESS, &read_callback);

  twi_write_byte (TWI_ID_RTC, TAG_DISABLE_32KHZ,
                  REG_ENABLE_32KHZ, REG_VALUE_DISABLE_32KHZ);

  watch_set (INITIAL_TIME, INITIAL_TIME, INITIAL_TIME);
}

ISR (INT2_vect)
{
  /* ! second should be the last one */
  /*
   * It would be better to use TAG_HOUR/MINUTE/SECOND here
   * but then we will need to convert tag to BUFFER_XXX in read_callback,
   * so lets try to use this approach
   */
  twi_read_byte (TWI_ID_RTC, BUFFER_HOUR, REG_HOUR);
  twi_read_byte (TWI_ID_RTC, BUFFER_MINUTE, REG_MINUTE);
  twi_read_byte (TWI_ID_RTC, BUFFER_SECOND, REG_SECOND);
  /* twi_read_array (TWI_ID_RTC, 0, BUFFER_SIZE, REG_SECOND); */
}
