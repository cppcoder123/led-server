/*
 *
 */

#include "unix/constant.h"

#include "display.h"
#include "encode.h"
#include "flush.h"
#include "queue.h"

#define MATRIX_SIZE 32

/* fixme: should we make less than max ?*/
#define LED_SIZE 255

volatile uint8_t led_data[LED_SIZE];

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
  queue_init (led_data, LED_SIZE);
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

uint8_t flush_push (uint8_t symbol)
{
  return queue_symbol_fill (led_data, symbol);
}

uint8_t flush_push_array (uint8_t *arr, uint8_t arr_size)
{
  return queue_array_fill (led_data, arr, arr_size);
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

  if (queue_size (led_data) < MATRIX_SIZE) {
    if (mode != FLUSH_SHIFT)
      return;
    if (global_mode == FLUSH_GLOBAL_ENABLED)
      encode_msg_1 (MSG_ID_POLL, SERIAL_ID_TO_IGNORE, 0);
    for (uint8_t i = 0; i < MATRIX_SIZE; ++i)
      queue_symbol_fill (led_data, 0);
  }

  display_data_start ();

  uint8_t symbol;
  for (uint8_t i = 0; i < MATRIX_SIZE; ++i)
    if (queue_symbol_get (led_data, i, &symbol) != 0)
      display_data_column (symbol);
    else
      display_data_column (1);

  display_data_stop ();

  if (mode == FLUSH_SHIFT)
    queue_symbol_drain (led_data, &symbol);
  else if (mode == FLUSH_CLEAR)
    queue_clear (led_data);

  mode = FLUSH_DISABLED;
}

/* void led_flush_mono (uint8_t *matrix) */
/* { */
/*   /\*fixme*\/ */
/* } */
