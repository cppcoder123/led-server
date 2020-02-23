/*
 *
 */

/* #include "buzz.h" */
#include "clock.h"
#include "debug.h"
#include "flush.h"
#include "font.h"
#include "render.h"

#define LONG_INDENT 22

#define BUFFER_SIZE FLUSH_STABLE_SIZE

#define ALARM_SPACE 3

static uint8_t second = 0;
static uint8_t minute = 0;
static uint8_t hour = 0;

static uint8_t alarm_engaged = 0;
static uint8_t alarm_minute = 0;
static uint8_t alarm_hour = 0;

void clock_init ()
{
  second = 0;
  minute = 0;
  hour = 0;

  alarm_engaged = 0;
  alarm_minute = 0;
  alarm_hour = 0;
}

void clock_set(uint8_t new_hour, uint8_t new_minute)
{
  hour = new_hour;
  minute = new_minute;
  second = 0;
}

void clock_get(uint8_t *hour_value, uint8_t *minute_value)
{
  *hour_value = hour;
  *minute_value = minute;
}

static uint8_t advance_second()
{
  /* debug_3 (15, 7, hour, minute, second); */

  if (second < CLOCK_SECOND_MAX) {
    ++second;
    return 0;
  }

  second = 0;
  if (minute < CLOCK_MINUTE_MAX) {
    ++minute;
    return 1;
  }

  minute = 0;
  if (hour < CLOCK_HOUR_MAX) {
    ++hour;
    return 1;
  }

  hour = 0;
  return 1;
}

void clock_advance_second()
{
  if (advance_second () == 0)
    return;

  if (alarm_engaged == 0)
    return;

  /* if ((alarm_hour == hour) */
  /*     && (alarm_minute == minute)) */
  /*   buzz_start(); */
}

uint8_t clock_alarm_set (uint8_t a_hour, uint8_t a_minute)
{
  if ((a_hour > CLOCK_HOUR_MAX)
      || (a_minute > CLOCK_MINUTE_MAX))
    return 0;

  alarm_hour = a_hour;
  alarm_minute = a_minute;

  return 1;
}

void clock_alarm_get (uint8_t *a_hour, uint8_t *a_min)
{
  *a_hour = alarm_hour;
  *a_min = alarm_minute;
}

void clock_alarm_engage_set(uint8_t engage)
{
  alarm_engaged = (engage == 0) ? 0 : 1;

  /* if (alarm_engaged == 0) */
  /*   buzz_stop (); */
}

uint8_t clock_alarm_engage_get ()
{
  return alarm_engaged;
}
  
void clock_render(volatile struct buf_t *buf)
{
  /* debug_2 (77, 123, alarm_hour, alarm_minute); */

  uint8_t indent = (alarm_engaged == 0) ? LONG_INDENT : 0;

  /* uint8_t position = 0; */
  for (uint8_t i = 0; i < indent; ++i)
    buf_byte_fill (buf, 0);

  render_number (buf, hour, RENDER_LEADING_DISABLE);
  render_symbol (buf, FONT_COLON);
  render_number (buf, minute, RENDER_LEADING_TEN);

  if (alarm_engaged != 0) {
    for (uint8_t j = 0; j < ALARM_SPACE; ++j)
      buf_byte_fill (buf, 0);
    /* render_symbol (FONT_AT, buffer, &position); */
    render_number (buf, alarm_hour, RENDER_LEADING_DISABLE);
    render_symbol (buf, FONT_COLON);
    render_number (buf, alarm_minute, RENDER_LEADING_TEN);
  }

  render_tail (buf);
}
