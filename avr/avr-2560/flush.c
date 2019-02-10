/*
 *
 */

#include "flush.h"
#include "flush-hw.h"
#include "ring.h"

#define MATRIX_SIZE 32

/* fixme: should we make less than max ?*/
#define MONO_SIZE 255

volatile data_t mono_data[MONO_SIZE];

enum {
  FLUSH_SHIFT,
  FLUSH_CLEAR,
  FLUSH_DISABLED
};
volatile uint8_t mode;

static void mode_change (uint8_t new_mode)
{
  if ((mode != FLUSH_DISABLED)
      || (ring_size (mono_data) < MATRIX_SIZE))
    return;

  mode = new_mode;
}

void flush_init ()
{
  /*fixme*/
  ring_init (mono_data, MONO_SIZE);
  mode = FLUSH_DISABLED;
}

uint8_t flush_push_mono (data_t symbol)
{
  return ring_symbol_fill (mono_data, symbol);
}

void flush_enable_shift ()
{
  mode_change (FLUSH_SHIFT);
}

void flush_enable_clear ()
{
  mode_change (FLUSH_CLEAR);
}

void flush_try ()
{
  if ((mode == FLUSH_DISABLED)
      || (ring_size (mono_data) < MATRIX_SIZE))
    return;

  flush_hw_mono_start ();

  uint8_t symbol;
  for (uint8_t i = 0; i < MATRIX_SIZE; ++i)
    if (ring_symbol_get (mono_data, i, &symbol) != 0)
      flush_hw_mono (symbol);
    else
      flush_hw_mono (1);

  flush_hw_mono_stop ();

  if (mode == FLUSH_SHIFT)
    ring_symbol_drain (mono_data, &symbol);
  else if (mode == FLUSH_CLEAR)
    ring_clear (mono_data);

  mode = FLUSH_DISABLED;
}

/* void led_flush_mono (data_t *matrix) */
/* { */
/*   /\*fixme*\/ */
/* } */
