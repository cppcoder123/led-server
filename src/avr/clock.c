/*
 *
 */

#include "buzz.h"
#include "clock.h"
#include "debug.h"
#include "flush.h"
#include "font.h"
#include "render.h"

/* it seems the clock is slower than needed */
#define FIFTY_NINE 58
#define TWENTY_THREE 23

#define LONG_INDENT 22
#define SHORT_INDENT 2

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

  if (second < FIFTY_NINE) {
    ++second;
    return 0;
  }

  second = 0;
  if (minute < FIFTY_NINE) {
    ++minute;
    return 1;
  }

  minute = 0;
  if (hour < TWENTY_THREE) {
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

  if ((alarm_hour == hour)
      && (alarm_minute == minute))
    buzz_start();
}

uint8_t clock_alarm_engage(uint8_t a_hour, uint8_t a_minute)
{
  if ((a_hour > TWENTY_THREE)
      || (a_minute > FIFTY_NINE))
    return 0;

  alarm_engaged = 1;
  alarm_hour = a_hour;
  alarm_minute = a_minute;

  return 1;
}

void clock_alarm_disengage()
{
  alarm_engaged = 0;
  alarm_hour = 0;
  alarm_minute = 0;

  buzz_stop();
}

void clock_render(uint8_t *buffer)
{
  uint8_t indent = (alarm_engaged == 0) ? LONG_INDENT : SHORT_INDENT;

  uint8_t position = 0;
  for (uint8_t i = 0; i < indent; ++i)
    render_empty_column (buffer, &position);

  render_number (hour, 0, buffer, &position);
  render_symbol (FONT_COLON, buffer, &position);
  render_number (minute, RENDER_LEADING_TEN, buffer, &position);

  if (alarm_engaged != 0) {
    for (uint8_t j = 0; j < ALARM_SPACE; ++j)
      render_empty_column (buffer, &position);
    render_symbol (FONT_AT, buffer, &position);
    render_number (alarm_hour, 0, buffer, &position);
    render_symbol (FONT_COLON, buffer, &position);
    render_number (alarm_minute, RENDER_LEADING_TEN, buffer, &position);
  }

  /* fill the tail with spaces */
  for (uint8_t j = position; j < BUFFER_SIZE; ++j)
    render_empty_column (buffer, &position);
}
