/*
 *
 */

#include "buzz.h"
#include "clock.h"
#include "debug.h"
#include "flush.h"
#include "font.h"
#include "render.h"

#define FIFTY_NINE 59
#define TWENTY_THREE 23
#define TEN 10

#define MINUTE_CHANGED 1
#define MINUTE_NOT_CHANGED 0

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
    return MINUTE_NOT_CHANGED;
  }

  second = 0;
  if (minute < FIFTY_NINE) {
    ++minute;
    return MINUTE_CHANGED;
  }

  minute = 0;
  if (hour < TWENTY_THREE) {
    ++hour;
    return MINUTE_CHANGED;
  }

  hour = 0;
  return MINUTE_CHANGED;
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

/* static void add_digit(uint8_t digit, uint8_t *buffer, uint8_t *position) */
/* { */
/*   uint8_t symbol = FONT_MAX_SYMBOL; */

/*   switch(digit) { */
/*   case 0: */
/*     symbol = FONT_0; */
/*     break; */
/*   case 1: */
/*     symbol = FONT_1; */
/*     break; */
/*   case 2: */
/*     symbol = FONT_2; */
/*     break; */
/*   case 3: */
/*     symbol = FONT_3; */
/*     break; */
/*   case 4: */
/*     symbol = FONT_4; */
/*     break; */
/*   case 5: */
/*     symbol = FONT_5; */
/*     break; */
/*   case 6: */
/*     symbol = FONT_6; */
/*     break; */
/*   case 7: */
/*     symbol = FONT_7; */
/*     break; */
/*   case 8: */
/*     symbol = FONT_8; */
/*     break; */
/*   case 9: */
/*     symbol = FONT_9; */
/*     break; */
/*   default: */
/*     symbol = FONT_Y; */
/*     break; */
/*   } */

/*   font_add_symbol(symbol, buffer, position, BUFFER_SIZE); */
/* } */

/* static void add_number(uint8_t number, uint8_t *buffer, */
/*                        uint8_t *position, uint8_t leading_zero) */
/* { */
/*   /\* number should be less than a hundred *\/ */
/*   uint8_t tmp = number / TEN; */
/*   if (tmp >= TEN) { */
/*     font_add_symbol(FONT_X, buffer, position, BUFFER_SIZE); */
/*     font_add_symbol(FONT_X, buffer, position, BUFFER_SIZE); */
/*     return; */
/*   } */

/*   if ((tmp != 0) */
/*       || (leading_zero != 0)) { */
/*     add_digit(tmp, buffer, position); */
/*     /\* small space between digits for better look *\/ */
/*     *(buffer + (*position)++) = 0; */
/*   } */

/*   tmp = number % TEN; */
/*   add_digit(tmp, buffer, position); */
/* } */

void clock_render(uint8_t *buffer)
{
  uint8_t indent = (alarm_engaged == 0) ? LONG_INDENT : SHORT_INDENT;

  uint8_t position = 0;
  for (uint8_t i = 0; i < indent; ++i)
    buffer[position++] = 0;

  render_number (hour, 0, buffer, &position);
  render_symbol (FONT_COLON, buffer, &position);
  render_number (minute, 1, buffer, &position);

  /* add_number(hour, buffer, &position, 0); */
  /* font_add_symbol(FONT_COLON, buffer, &position, BUFFER_SIZE); */
  /* add_number(minute, buffer, &position, 1); */

  if (alarm_engaged != 0) {
    for (uint8_t j = 0; j < ALARM_SPACE; ++j)
      render_empty_column (buffer, &position);
    render_symbol (FONT_AT, buffer, &position);
    render_number (alarm_hour, 0, buffer, &position);
    render_symbol (FONT_COLON, buffer, &position);
    render_number (alarm_minute, 1, buffer, &position);
    /* font_add_symbol(FONT_SPACE, buffer, &position, BUFFER_SIZE); */
    /* font_add_symbol(FONT_AT, buffer, &position, BUFFER_SIZE); */
    /* add_number(alarm_hour, buffer, &position, 0); */
    /* font_add_symbol(FONT_COLON, buffer, &position, BUFFER_SIZE); */
    /* add_number(alarm_minute, buffer, &position, 1); */
  }


  /* fill the tail with spaces */
  for (uint8_t j = position; j < BUFFER_SIZE; ++j)
    buffer[position++] = 0;
}
