/*
 *
 */

#include <avr/io.h>

#include "clock.h"
#include "debug.h"
#include "flush.h"
#include "invoke.h"
#include "power.h"

#define MASTER_DELAY 50
#define SLAVE_DELAY 0

#define MASTER_BUFFER_SIZE 64

static uint8_t mode = POWER_UNKNOWN;

static void advance_clock ()
{
  clock_advance_second ();

  uint8_t buffer[MASTER_BUFFER_SIZE];
  clock_render (buffer);
  /* ? */
  flush_buffer_clear ();
  flush_push_array (buffer, MASTER_BUFFER_SIZE);
  flush_stable_display ();


}

static void start_master ()
{
  flush_stable_enable ();
  invoke_enable (INVOKE_ID_POWER, MASTER_DELAY, &advance_clock);
}

static void stop_master ()
{
  invoke_disable (INVOKE_ID_POWER);
  flush_stable_disable ();
}

static void start_slave ()
{
    flush_shift_enable();
}

static void stop_slave ()
{
  flush_shift_disable ();
}

void power_init ()
{
  /* fixme: Configure power wire as output and set it to 0 */
  mode = POWER_UNKNOWN;
}

void power_set_mode (uint8_t new_mode)
{
  if (new_mode == mode)
    return;

  if (new_mode == POWER_MASTER) {
    stop_slave ();
    start_master ();
  } else {
    stop_master ();
    start_slave ();
  }
}

uint8_t power_get_mode ()
{
  return mode;
}
