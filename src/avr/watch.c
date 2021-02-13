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

#define REG_HOUR 2
#define REG_MINUTE 1
#define REG_SECOND 0

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
      /* BUFFER_WRITE_HOUR, */
      /* BUFFER_WRITE_MINUTE, */
      /* BUFFER_WRITE_SECOND, */
      BUFFER_HOUR,
      BUFFER_MINUTE,
      BUFFER_SECOND,
      BUFFER_SIZE               /* ! Keep it last */
};

static uint8_t buffer[BUFFER_SIZE];

/*
 * Event: what we want to handle
 */
enum {                          /* we are doing nothing */
      EVENT_ENABLE,
      EVENT_DISABLE,
      EVENT_DISABLE_32KHZ,
      EVENT_HOUR,
      EVENT_MINUTE,
      EVENT_SECOND,
};

/* static uint8_t event;           /\* event we are handling now   *\/ */

/* enum { */
/*       SUB_EVENT_IDLE, */
/*       SUB_EVENT_HOUR,            /\* hanle it one at a time *\/ */
/*       SUB_EVENT_MINUTE, */
/*       SUB_EVENT_SECOND, */
/* }; */

/* static uint8_t sub_event; */

/* enum { */
/*       ACTION_IDLE, */
/*       ACTION_READY, */
/*       ACTION_IN_PROGRESS, */
/* }; */

/* static uint8_t action; */

/* static struct buf_t event_queue; */

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

static void init_interrupt ()
{
   /* 1 & 1 => rising edge */
  EICRA |= ((1 << ISC21) | (1 << ISC20));
  /* enable INT2 interrupt */
  EIMSK |= (1 << INT2);
}

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

static void write_callback (uint8_t event, uint8_t status)
{
  /* do we need to handle something here? */
}
/*   if (status != TWI_SUCCESS) */
/*     return; */

/*   action = ACTION_IDLE; */

/*   if ((event == EVENT_ENABLE) */
/*       || (event == EVENT_DISABLE) */
/*       || (event == EVENT_DISABLE_32KHZ)) { */
/*     event = EVENT_IDLE; */
/*     return; */
/*   } */

/*   if (event == EVENT_WRITE) { */
/*     if (sub_event == SUB_EVENT_SECOND) { */
/*       event = EVENT_IDLE; */
/*       sub_event = SUB_EVENT_IDLE; */
/*       return; */
/*     } */

/*     if (sub_event == SUB_EVENT_HOUR) */
/*       sub_event = SUB_EVENT_MINUTE; */
/*     else if (sub_event == SUB_EVENT_MINUTE) */
/*       sub_event = SUB_EVENT_SECOND; */
/*     /\* !!! action is not idle, we should continue *\/ */
/*     action = ACTION_READY; */
/*   } */
/* } */

/* static void write () */
/* { */
/*   uint8_t reg = REG_ENABLE; */
/*   uint8_t reg_value = REG_VALUE_ENABLE; */

/*   switch (event) { */
/*   case EVENT_ENABLE: */
/*   case EVENT_DISABLE: */
/*     reg_value = (event == EVENT_ENABLE) ? REG_VALUE_ENABLE */
/*       : REG_VALUE_DISABLE; */
/*     break; */
/*   case EVENT_DISABLE_32KHZ: */
/*     reg = REG_ENABLE_32KHZ; */
/*     reg_value = REG_VALUE_DISABLE_32KHZ; */
/*     break; */
/*   case EVENT_WRITE: */
/*     if (sub_event == SUB_EVENT_HOUR) { */
/*       reg = REG_HOUR; */
/*       reg_value = buffer[BUFFER_WRITE_HOUR]; */
/*     } else if (sub_event == SUB_EVENT_MINUTE) { */
/*       reg = REG_MINUTE; */
/*       reg_value = buffer[BUFFER_WRITE_MINUTE]; */
/*     } else if (sub_event == SUB_EVENT_SECOND) { */
/*       reg = REG_SECOND; */
/*       reg_value = buffer[BUFFER_WRITE_SECOND]; */
/*     } */
/*     break; */
/*   default: */
/*     break; */
/*   } */

/*   twi_write_byte (TWI_ID_RTC, reg, reg_value); */
/* } */

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

static void read_callback (uint8_t event, uint8_t status, uint8_t value)
{
  if (status != TWI_SUCCESS)
    return;

  switch (event) {
  case EVENT_HOUR:
    buffer[BUFFER_HOUR] = value;
    break;
  case EVENT_MINUTE:
    buffer[BUFFER_MINUTE] = value;
    break;
  case EVENT_SECOND:
    buffer[BUFFER_SECOND] = value;
    if ((value == 0)
        && (watch_alarm_state ()))
      alarm_check (buffer[BUFFER_HOUR], buffer[BUFFER_MINUTE]);
    render ();
    break;
  default:
    /* we are reading only time */
    break;
  }
}

/*   action = ACTION_IDLE; */

/*   if ((event == EVENT_ENABLE) */
/*       || (event == EVENT_DISABLE)) { */
/*     /\* do we need this ? *\/ */
/*     event = EVENT_IDLE; */
/*     return; */
/*   } */

/*   if (event == EVENT_READ) { */
/*     switch (sub_event) { */
/*     case SUB_EVENT_HOUR: */
/*       buffer[BUFFER_HOUR] = value; */
/*       break; */
/*     case SUB_EVENT_MINUTE: */
/*       buffer[BUFFER_MINUTE] = value; */
/*       break; */
/*     case SUB_EVENT_SECOND: */
/*       buffer[BUFFER_SECOND] = value; */
/*       if ((value == 0) */
/*           && (watch_alarm_state ())) */
/*         alarm_check (buffer[BUFFER_HOUR], buffer[BUFFER_MINUTE]); */
/*       break; */
/*     default: */
/*       break; */
/*     } */
/*   } */

/*   if (sub_event == SUB_EVENT_SECOND) { */
/*     render (); */
/*     event = EVENT_IDLE; */
/*     sub_event = SUB_EVENT_IDLE; */
/*     return; */
/*   } */

/*   if (sub_event == SUB_EVENT_HOUR) */
/*     sub_event = SUB_EVENT_MINUTE; */
/*   else if (sub_event == SUB_EVENT_MINUTE) */
/*     sub_event = SUB_EVENT_SECOND; */
/*   /\* action is not idle, we should continue *\/ */
/*   action = ACTION_READY; */
/* } */

/* static void read () */
/* { */
/*   if (event != EVENT_READ) */
/*     return; */

/*   uint8_t reg = (sub_event == SUB_EVENT_HOUR) */
/*     ? REG_HOUR */
/*     : (sub_event == SUB_EVENT_MINUTE) */
/*     ? REG_MINUTE */
/*     : REG_SECOND; */

/*   twi_read_byte (TWI_ID_RTC, reg); */
/* } */

/* void watch_try () */
/* { */
/*   /\* */
/*    * 1. We need to check whether we have something ready for processing */
/*    *   or something is in progress */
/*    * 2. pull new event from queue */
/*    *\/ */
/*   if (event != EVENT_IDLE) { */
/*     if (action == ACTION_READY) { */
/*       /\**\/ */
/*       action = ACTION_IN_PROGRESS; */
/*       /\**\/ */
/*       if (event == EVENT_READ) */
/*         read (); */
/*       else if ((event == EVENT_ENABLE) */
/*                || (event == EVENT_DISABLE) */
/*                || (event == EVENT_DISABLE_32KHZ) */
/*                || (event == EVENT_WRITE)) */
/*         write (); */
/*     } */
/*     return; */
/*   } */

/*   uint8_t new_event; */
/*   if (buf_byte_drain (&event_queue, &new_event) == 0) */
/*     return;                     /\* empty *\/ */

/*   event = new_event; */

/*   if ((event == EVENT_READ) */
/*       || (event == EVENT_WRITE)) */
/*     sub_event = SUB_EVENT_HOUR; */

/*   action = ACTION_READY; */
/* } */

void watch_enable ()
{
  /* buf_byte_fill (&event_queue, EVENT_ENABLE); */
  twi_write_byte (TWI_ID_RTC, EVENT_ENABLE, REG_ENABLE, REG_VALUE_ENABLE);
}

void watch_disable ()
{
  /* buf_byte_fill (&event_queue, EVENT_DISABLE); */
  twi_write_byte (TWI_ID_RTC, EVENT_DISABLE, REG_ENABLE, REG_VALUE_DISABLE);
}

void watch_set (uint8_t hour, uint8_t minute, uint8_t second)
{
  /* buffer[BUFFER_WRITE_HOUR] = rtc (hour, RTC_TO, RTC_HOUR); */
  /* buffer[BUFFER_WRITE_MINUTE] = rtc (minute, RTC_TO, RTC_MINUTE); */
  /* buffer[BUFFER_WRITE_SECOND] = rtc (second, RTC_TO, RTC_SECOND); */

  /* buf_byte_fill (&event_queue, EVENT_WRITE); */

  twi_write_byte (TWI_ID_RTC, EVENT_HOUR,
                  REG_HOUR, rtc (hour, RTC_TO, RTC_HOUR));
  twi_write_byte (TWI_ID_RTC, EVENT_MINUTE,
                  REG_MINUTE, rtc (minute, RTC_TO, RTC_MINUTE));
  twi_write_byte (TWI_ID_RTC, EVENT_SECOND,
                  REG_SECOND, rtc (second, RTC_TO, RTC_SECOND));
}

void watch_get (uint8_t *hour, uint8_t *minute)
{
  /* fixme: remove this */
  *hour = 11;
  *minute = 11;
}

void watch_init ()
{
  for (uint8_t i = 0; i < BUFFER_SIZE; ++i)
    buffer[i] = 0;
  /* event = EVENT_IDLE; */
  /* sub_event = SUB_EVENT_IDLE; */
  /* action = ACTION_IDLE; */
  /* buf_init (&event_queue); */
  init_interrupt ();            /* register for 1Hz signal */

  /* buf_byte_fill (&event_queue, EVENT_DISABLE_32KHZ); */

  /* buffer[BUFFER_WRITE_HOUR] = rtc (INITIAL_TIME, RTC_TO, RTC_HOUR); */
  /* buffer[BUFFER_WRITE_MINUTE] = rtc (INITIAL_TIME, RTC_TO, RTC_MINUTE); */
  /* buffer[BUFFER_WRITE_SECOND] = rtc (INITIAL_TIME, RTC_TO, RTC_SECOND); */
  /* buf_byte_fill (&event_queue, EVENT_WRITE); */
  watch_set (INITIAL_TIME, INITIAL_TIME, INITIAL_TIME);

  watch_alarm_set (0, 0);
  watch_alarm_control (0);      /* disengage */

  twi_slave (TWI_ID_RTC, RTC_ADDRESS, &write_callback, &read_callback);
}

ISR (INT2_vect)
{
  /* we need to read new time value */
  /* buf_byte_fill (&event_queue, EVENT_READ); */

  /* ! second should be last one */
  twi_read_byte (TWI_ID_RTC, EVENT_HOUR, REG_HOUR);
  twi_read_byte (TWI_ID_RTC, EVENT_MINUTE, REG_MINUTE);
  twi_read_byte (TWI_ID_RTC, EVENT_SECOND, REG_SECOND);
}
