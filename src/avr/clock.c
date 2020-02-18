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

static uint8_t my_alarm_engaged = 0;
static uint8_t my_alarm_minute = 0;
static uint8_t my_alarm_hour = 0;

void clock_init ()
{
  second = 0;
  minute = 0;
  hour = 0;

  my_alarm_engaged = 0;
  my_alarm_minute = 0;
  my_alarm_hour = 0;
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

  if (my_alarm_engaged == 0)
    return;

  /* if ((my_alarm_hour == hour) */
  /*     && (my_alarm_minute == minute)) */
  /*   buzz_start(); */
}

uint8_t clock_alarm_set (uint8_t a_hour, uint8_t a_minute)
{
  if ((a_hour > CLOCK_HOUR_MAX)
      || (a_minute > CLOCK_MINUTE_MAX))
    return 0;

  my_alarm_hour = a_hour;
  my_alarm_minute = a_minute;

  return 1;
}

void clock_alarm_get (uint8_t *a_hour, uint8_t *a_min)
{
  *a_hour = my_alarm_hour;
  *a_min = my_alarm_minute;
}

void clock_alarm_engage_set(uint8_t engage)
{
  my_alarm_engaged = (engage == 0) ? 0 : 1;

  /* if (my_alarm_engaged == 0) */
  /*   buzz_stop (); */
}

uint8_t clock_alarm_engage_get ()
{
  return my_alarm_engaged;
}
  
void clock_render(uint8_t *buffer)
{
  debug_2 (77, 123, my_alarm_hour, my_alarm_minute);

  uint8_t indent = (my_alarm_engaged == 0) ? LONG_INDENT : 0;

  uint8_t position = 0;
  for (uint8_t i = 0; i < indent; ++i)
    render_empty_column (buffer, &position);

  render_number (hour, 0, buffer, &position);
  render_symbol (FONT_COLON, buffer, &position);
  render_number (minute, RENDER_LEADING_TEN, buffer, &position);

  if (my_alarm_engaged != 0) {
    for (uint8_t j = 0; j < ALARM_SPACE; ++j)
      render_empty_column (buffer, &position);
    /* render_symbol (FONT_AT, buffer, &position); */
    render_number (my_alarm_hour, 0, buffer, &position);
    render_symbol (FONT_COLON, buffer, &position);
    render_number (my_alarm_minute, RENDER_LEADING_TEN, buffer, &position);
  }

  /* fill the tail with spaces */
  for (uint8_t j = position; j < BUFFER_SIZE; ++j)
    render_empty_column (buffer, &position);
}
