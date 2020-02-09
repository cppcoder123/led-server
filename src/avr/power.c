/*
 *
 */

#include <avr/io.h>

#include "clock.h"
#include "counter.h"
#include "flush.h"
#include "poll.h"
#include "power.h"
#include "spi.h"

#define TEN_PER_SECOND_LOW 100
#define TEN_PER_SECOND_HIGH 0

#define ONE_PER_SECOND_LOW 0
#define ONE_PER_SECOND_HIGH 61

#define POWER_COUNTER COUNTER_3

#define MASTER_BUFFER_SIZE 64

typedef void (*timer_callback) ();

static uint8_t mode = POWER_UNKNOWN;

static void timer_engage (uint8_t low, uint8_t high, timer_callback callback)
{
  counter_interrupt (POWER_COUNTER, COUNTER_INTERRUPT_COMPARE_A, callback);
  counter_set_compare_a (POWER_COUNTER, low, high);
  counter_enable (POWER_COUNTER, COUNTER_PRESCALER_1024);
}

static void timer_relax ()
{
  counter_disable (POWER_COUNTER);
}

static void advance_clock ()
{
  clock_advance_second ();

  uint8_t buffer[MASTER_BUFFER_SIZE];
  clock_render (buffer);
  flush_push_array (buffer, MASTER_BUFFER_SIZE);
  flush_enable_clear ();
}

static void start_master ()
{
  timer_engage (ONE_PER_SECOND_LOW, ONE_PER_SECOND_HIGH, &advance_clock);
}

static void stop_master ()
{
  timer_relax ();
}

static void start_slave ()
{
  timer_engage (TEN_PER_SECOND_LOW, TEN_PER_SECOND_HIGH, &flush_enable_shift);
}

static void stop_slave ()
{
  timer_relax ();
}

static void disconnect_callback ()
{
  power_set_mode (POWER_MASTER);
}

void power_init ()
{
  /* fixme: Configure power wire as output and set 0 there */
  mode = POWER_UNKNOWN;

  spi_note_disconnect (disconnect_callback);
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
