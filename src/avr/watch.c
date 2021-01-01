/*
 *
 */

#include "buf.h"
#include "twi.h"
#include "watch.h"

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

/* 1 enable, 0 disable */
static void configure (uint8_t arg/*event_enable/disable*/)
{
  /* fixme */
}

void watch_init ()
{
  /* fixme */
}

static void write ()
{
  /* fixme */
}

static void read_callback (uint8_t status, uint8_t value)
{
  /*fixme*/
  if (status != TWI_SUCCESS) {
    /*fixme debug*/
    return;
  }

  action = ACTION_IDLE;

  if ((event == EVENT_ENABLE)
      || (event == EVENT_DISABLE)) {
    event = EVENT_IDLE;
    return;
  }

  if (sub_event == SUB_EVENT_SECOND) {
    /*render () fixme */ 
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

/* 'read_reg' should be called from local _try function */
/* !!! Redo */
static void read ()
{
  /* fixme */

  twi_read_byte (111/*REG fixme*/, &read_callback);
  
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
        configure (event);
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

  /* fixme */
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
