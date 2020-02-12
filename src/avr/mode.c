/*
 *
 */
#include "clock.h"
#include "flush.h"
#include "invoke.h"
#include "heartbeat.h"
#include "mode.h"
#include "spi.h"

#define CLOCK_DELAY 50          /* 1 sec ? */
#define MASTER_DELAY 250        /* 5 sec ? */
#define SLAVE_DELAY 0           /* 0.02 sec */

#define HB_DELAY 50     	/* 1 sec ? */
#define HB_MISS 3		/* we can miss 3 heartbeats */

static uint8_t current_mode = MODE_IDLE;

void mode_init ()
{
  current_mode = MODE_IDLE;
}

static void stop_mode ()
{
  switch (current_mode) {
  case MODE_IDLE:
    break;
  case MODE_MASTER:
    invoke_disable (INVOKE_ID_FLUSH);
    break;
  case MODE_SLAVE:
    invoke_disable (INVOKE_ID_FLUSH);
    invoke_disable (INVOKE_ID_CLOCK);
    heartbeat_cancel ();
    break;
  default:
    break;
  }
}

static void render_clock ()
{
  uint8_t matrix[64];
  clock_render (matrix);
  flush_stable_data (matrix);
  flush_stable_display ();
}

static void switch_to_master ()
{
  mode_set (MODE_MASTER);
}

static void start_mode ()
{
  switch (current_mode) {
  case MODE_IDLE:
    break;
  case MODE_MASTER:
    invoke_enable (INVOKE_ID_FLUSH, MASTER_DELAY, &render_clock);
    invoke_enable (INVOKE_ID_CLOCK, CLOCK_DELAY, &clock_advance_second);
    break;
  case MODE_SLAVE:
    invoke_enable (INVOKE_ID_FLUSH, SLAVE_DELAY, &flush_shift_display);
    heartbeat_start (HB_DELAY, HB_MISS, &switch_to_master, &spi_interrupt_start);
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
