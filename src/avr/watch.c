/*
 *
 */
#include <avr/interrupt.h>
#include <avr/io.h>

#include "unix/constant.h"

#include "buf.h"
#include "debug.h"
#include "flush.h"
#include "font.h"
#include "render.h"
#include "twi.h"
#include "watch.h"

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
 *   => 8 disables 32kHz
 */
#define REG_VALUE_DISABLE_32KHZ 0

/* set initial time */
#define INITIAL_TIME 1

/* 10 pixels from left */
#define IMAGE_INDENT 10

/*
 * Buffer, either read or write
 */
enum {
      BUFFER_WRITE_HOUR,
      BUFFER_WRITE_MINUTE,
      BUFFER_WRITE_SECOND,
      BUFFER_READ_HOUR,
      BUFFER_READ_MINUTE,
      BUFFER_READ_SECOND,
      BUFFER_SIZE               /* ! Keep it last */
};

static uint8_t buffer[BUFFER_SIZE];

/*
 * Event: what we want to handle
 */
enum {
      EVENT_IDLE,               /* we are doing nothing */
      EVENT_ENABLE,
      EVENT_DISABLE,
      EVENT_DISABLE_32KHZ,
      EVENT_WRITE,
      EVENT_READ,
};

static uint8_t event;           /* event we are handling now   */

enum {
      SUB_EVENT_IDLE,
      SUB_EVENT_HOUR,            /* hanle it one at a time */
      SUB_EVENT_MINUTE,
      SUB_EVENT_SECOND,
};

static uint8_t sub_event;

enum {
      ACTION_IDLE,
      ACTION_READY,
      ACTION_IN_PROGRESS,
};

static uint8_t action;

static struct buf_t event_queue;

static void init_interrupt ()
{
   /* 1 & 1 => rising edge */
  EICRA |= ((1 << ISC21) | (1 << ISC20));
  /* enable INT2 interrupt */
  EIMSK |= (1 << INT2);
}


void watch_init ()
{
  for (uint8_t i = 0; i < BUFFER_SIZE; ++i)
    buffer[i] = 0;
  event = EVENT_IDLE;
  sub_event = SUB_EVENT_IDLE;
  action = ACTION_IDLE;
  buf_init (&event_queue);
  init_interrupt ();            /* register for 1Hz signal */

  buf_byte_fill (&event_queue, EVENT_DISABLE_32KHZ);

  buffer[BUFFER_WRITE_HOUR] = INITIAL_TIME;
  buffer[BUFFER_WRITE_MINUTE] = INITIAL_TIME;
  buffer[BUFFER_WRITE_SECOND] = INITIAL_TIME;
  buf_byte_fill (&event_queue, EVENT_WRITE);
}

static void write_callback (uint8_t status)
{
  if (status != TWI_SUCCESS) {
    debug_1 (DEBUG_TWI, 1, status);
    return;
  }

  action = ACTION_IDLE;

  if ((event == EVENT_ENABLE)
      || (event == EVENT_DISABLE)
      || (event == EVENT_DISABLE_32KHZ)) {
    event = EVENT_IDLE;
    return;
  }

  if (event == EVENT_WRITE) {
    if (sub_event == SUB_EVENT_SECOND) {
      event = EVENT_IDLE;
      sub_event = SUB_EVENT_IDLE;
      return;
    }

    if (sub_event == SUB_EVENT_HOUR)
      sub_event = SUB_EVENT_MINUTE;
    else if (sub_event == SUB_EVENT_MINUTE)
      sub_event = SUB_EVENT_SECOND;
    /* !!! action is not idle, we should continue */
    action = ACTION_READY;
  }
}

static void write ()
{
  uint8_t reg = REG_ENABLE;
  uint8_t reg_value = REG_VALUE_ENABLE;

  switch (event) {
  case EVENT_ENABLE:
  case EVENT_DISABLE:
    reg_value = (event == EVENT_ENABLE) ? REG_VALUE_ENABLE
      : REG_VALUE_DISABLE;
    break;
  case EVENT_DISABLE_32KHZ:
    reg = REG_ENABLE_32KHZ;
    reg_value = REG_VALUE_DISABLE_32KHZ;
    break;
  case EVENT_READ:
  case EVENT_WRITE:
    if (sub_event == SUB_EVENT_HOUR) {
      reg = REG_HOUR;
      reg_value = buffer[BUFFER_WRITE_HOUR];
    } else if (sub_event == SUB_EVENT_MINUTE) {
      reg = REG_MINUTE;
      reg_value = buffer[BUFFER_WRITE_MINUTE];
    } else if (sub_event == SUB_EVENT_SECOND) {
      reg = REG_SECOND;
      reg_value = buffer[BUFFER_WRITE_SECOND];
    }
    break;
  default:
    break;
  }

  twi_write_byte (reg, reg_value, &write_callback);
}

static void render () /* send watch value into display */
{
  struct buf_t image;
  buf_init (&image);

  for (uint8_t i = 0; i < IMAGE_INDENT; ++i)
    buf_byte_fill (&image, 0);

  render_number (&image, buffer[BUFFER_READ_HOUR], RENDER_LEADING_DISABLE);
  render_symbol (&image, FONT_COLON);
  render_number (&image, buffer[BUFFER_READ_MINUTE], RENDER_LEADING_TEN);
  render_symbol (&image, FONT_COLON);
  render_number (&image, buffer[BUFFER_READ_SECOND], RENDER_LEADING_TEN);

  render_tail (&image);

  flush_stable_display (&image);
}

static void read_callback (uint8_t status, uint8_t value)
{
  if (status != TWI_SUCCESS) {
    debug_1 (DEBUG_TWI, 0, status);
    return;
  }

  action = ACTION_IDLE;

  if ((event == EVENT_ENABLE)
      || (event == EVENT_DISABLE)) {
    /* do we need this ? */
    event = EVENT_IDLE;
    return;
  }

  if (event == EVENT_READ) {
    switch (sub_event) {
    case SUB_EVENT_HOUR:
      buffer[BUFFER_READ_HOUR] = value;
      break;
    case SUB_EVENT_MINUTE:
      buffer[BUFFER_READ_MINUTE] = value;
      break;
    case SUB_EVENT_SECOND:
      buffer[BUFFER_READ_SECOND] = value;
      break;
    default:
      break;
    }
  }

  if (sub_event == SUB_EVENT_SECOND) {
    render ();
    event = EVENT_IDLE;
    sub_event = SUB_EVENT_IDLE;
    return;
  }

  if (sub_event == SUB_EVENT_HOUR)
    sub_event = SUB_EVENT_MINUTE;
  else if (sub_event == SUB_EVENT_MINUTE)
    sub_event = SUB_EVENT_SECOND;
  /* action is not idle, we should continue */
  action = ACTION_READY;
}

static void read ()
{
  if (event != EVENT_READ)
    return;

  uint8_t reg = (sub_event == SUB_EVENT_HOUR)
    ? REG_HOUR
    : (sub_event == SUB_EVENT_MINUTE)
    ? REG_MINUTE
    : REG_SECOND;

  twi_read_byte (reg, &read_callback);
}

void watch_try ()
{
  /*
   * 1. We need to check whether we have something ready for processing
   *   or something is in progress
   * 2. pull new event from queue
   */
  if (event != EVENT_IDLE) {
    if (action == ACTION_READY) {
      /**/
      action = ACTION_IN_PROGRESS;
      /**/
      switch (event) {
      case EVENT_ENABLE:
      case EVENT_DISABLE:
      case EVENT_DISABLE_32KHZ:
        write ();
        break;
      case EVENT_READ:
        read ();
        break;
      case EVENT_WRITE:
        write ();
        break;
      default:
        /* fixme: debug */
        break;
      }
    }
    return;
  }

  uint8_t new_event;
  if (buf_byte_drain (&event_queue, &new_event) == 0)
    return;                     /* empty */

  event = new_event;

  if ((event == EVENT_READ)
      || (event == EVENT_WRITE))
    sub_event = SUB_EVENT_HOUR;

  action = ACTION_READY;
}

void watch_enable ()
{
  buf_byte_fill (&event_queue, EVENT_ENABLE);
}

void watch_disable ()
{
  buf_byte_fill (&event_queue, EVENT_DISABLE);
}

void watch_write (uint8_t hour, uint8_t minute, uint8_t second)
{
  buffer[BUFFER_WRITE_HOUR] = hour;
  buffer[BUFFER_WRITE_MINUTE] = minute;
  buffer[BUFFER_WRITE_SECOND] = second;

  buf_byte_fill (&event_queue, EVENT_WRITE);
}

ISR (INT2_vect)
{
  /* we need to read new time value */
  buf_byte_fill (&event_queue, EVENT_READ);
}
