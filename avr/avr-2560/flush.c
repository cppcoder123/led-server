/*
 *
 */

#include "mcu/constant.h"

#include "encode.h"
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

enum {
  FLUSH_GLOBAL_DISABLED,
  FLUSH_GLOBAL_ENABLED
};
volatile uint8_t global_mode;

static void mode_change (uint8_t new_mode)
{
  if (mode != FLUSH_DISABLED)
    return;

  mode = new_mode;
}

void flush_init ()
{
  /*fixme*/
  ring_init (mono_data, MONO_SIZE);
  mode = FLUSH_DISABLED;
  global_mode = FLUSH_GLOBAL_DISABLED;
}

void flush_enable ()
{
  global_mode = FLUSH_GLOBAL_ENABLED;
}

void flush_disable ()
{
  global_mode = FLUSH_GLOBAL_DISABLED;
}

uint8_t flush_push_mono (data_t symbol)
{
  return ring_symbol_fill (mono_data, symbol);
}

uint8_t flush_push_mono_array (data_t *arr, uint8_t arr_size)
{
  return ring_array_fill (mono_data, arr, arr_size);
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
  if (mode == FLUSH_DISABLED)
    /*we are not ready*/
    return;

  if (ring_size (mono_data) < MATRIX_SIZE) {
    if (mode != FLUSH_SHIFT)
      return;
    if (global_mode == FLUSH_GLOBAL_ENABLED)
      encode_msg_1 (MSG_ID_POLL, SERIAL_ID_TO_IGNORE, 0);
    for (uint8_t i = 0; i < MATRIX_SIZE; ++i)
      ring_symbol_fill (mono_data, 0);
  }

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
