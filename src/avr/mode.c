/*
 *
 */
#include "buzz.h"
#include "cron.h"
#include "flush.h"
#include "heartbeat.h"
#include "mode.h"
#include "spi.h"
#include "watch.h"

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

static void switch_to_clock ()
{
  mode_set (MODE_CLOCK);
}

uint8_t mode_get ()
{
  return current_mode;
}

void mode_set (uint8_t new_mode)
{
  if (current_mode == new_mode)
    return;

  /*
   * Disable/stop everything
   */
  watch_disable ();
  heartbeat_cancel ();
  cron_disable (CRON_ID_FLUSH);
  is_connected = 0;

  /*
   * Enable new functionality
   */
  current_mode = new_mode;
  if (current_mode == MODE_CLOCK) {
    watch_enable ();
  } else if (current_mode == MODE_RADIO) {
    cron_enable (CRON_ID_FLUSH, SLAVE_DELAY, &flush_shift_display);
    heartbeat_start (HB_DELAY, HB_MISS, &switch_to_clock, &spi_interrupt_start);
    is_connected = 1;
  }
}

uint8_t mode_is_connnected ()
{
  return is_connected;
}
