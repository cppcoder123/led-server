/*
 *
 */
#include "clock.h"
#include "cron.h"
#include "flush.h"
#include "heartbeat.h"
#include "mode.h"
#include "spi.h"

#define CLOCK_DELAY 50          /* 1 sec ? */
#define CLOCK_RENDER_DELAY 250  /* 5 sec ? */
#define SLAVE_DELAY 0           /* 0.02 sec */

#define HB_DELAY 50     	/* 1 sec ? */
#define HB_MISS 3		/* we can miss 3 heartbeats */

static uint8_t current_mode = MODE_MENU;
static uint8_t is_connected = 0;

void mode_init ()
{
  current_mode = MODE_MENU;
  is_connected = 0;
}

static void stop_mode ()
{
  switch (current_mode) {
  case MODE_MENU:
    break;
  case MODE_CLOCK:
    cron_disable (CRON_ID_FLUSH);
    break;
  case MODE_RADIO:
    heartbeat_cancel ();
    cron_disable (CRON_ID_FLUSH);
    break;
  default:
    break;
  }
}

static void render_clock ()
{
  struct buf_t buf;
  buf_init (&buf);

  clock_render (&buf);
  flush_stable_display (&buf);
}

static void switch_to_clock ()
{
  mode_set (MODE_CLOCK);
  is_connected = 0;
}

static void start_mode ()
{
  switch (current_mode) {
  case MODE_MENU:
    cron_enable (CRON_ID_CLOCK, CLOCK_DELAY, &clock_advance_second);
    break;
  case MODE_CLOCK:
    cron_enable (CRON_ID_FLUSH, CLOCK_RENDER_DELAY, &render_clock);
    cron_enable (CRON_ID_CLOCK, CLOCK_DELAY, &clock_advance_second);
    break;
  case MODE_RADIO:
    cron_disable (CRON_ID_CLOCK);
    cron_enable (CRON_ID_FLUSH, SLAVE_DELAY, &flush_shift_display);
    heartbeat_start (HB_DELAY, HB_MISS, &switch_to_clock, &spi_interrupt_start);
    is_connected = 1;
    break;
  default:
    break;
  }
}

void mode_set (uint8_t new_mode)
{
  if (current_mode == new_mode)
    return;

  stop_mode ();
  current_mode = new_mode;
  start_mode ();
}

uint8_t mode_get ()
{
  return current_mode;
}

uint8_t mode_is_connnected ()
{
  return is_connected;
}
