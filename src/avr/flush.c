/*
 *
 */

#include <avr/io.h>

#include "unix/constant.h"

#include "buffer.h"
#include "debug.h"
#include "display.h"
#include "encode.h"
#include "flush.h"

#define MATRIX_SIZE 32
#define MIN_DATA_SIZE (MATRIX_SIZE * 2)

static volatile struct buffer_t led_data;

static struct display_t display_right;
static struct display_t display_left;

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
  buffer_init (&led_data);
  mode = FLUSH_DISABLED;
  global_mode = FLUSH_GLOBAL_DISABLED;

  display_init (&display_left, PORTA0, PORTA2, PORTA4);
  display_init (&display_right, PORTA1, PORTA3, PORTA5);
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
  return buffer_byte_fill (&led_data, symbol);
}

uint8_t flush_push_array (uint8_t *arr, uint8_t arr_size)
{
  return buffer_array_fill (&led_data, arr, arr_size);
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
  if ((global_mode == FLUSH_GLOBAL_DISABLED)
      || (mode == FLUSH_DISABLED)
      || (buffer_size (&led_data) < MIN_DATA_SIZE))
    return;

  display_data_start (&display_left);
  display_data_start (&display_right);

  uint8_t symbol;
  for (uint8_t i = 0; i < MATRIX_SIZE; ++i) {
    if (buffer_byte_get (&led_data, i, &symbol) == 0)
      symbol = 0;
    display_data_column (&display_left, symbol);
    if (buffer_byte_get (&led_data, i + MATRIX_SIZE, &symbol) == 0)
      symbol = 0;
    display_data_column (&display_right, symbol);
  }

  display_data_stop (&display_left);
  display_data_stop (&display_right);

  if (mode == FLUSH_SHIFT) {
    buffer_byte_drain (&led_data, &symbol);
  } else if (mode == FLUSH_CLEAR)
    buffer_clear (&led_data);

  mode = FLUSH_DISABLED;
}

uint8_t flush_buffer_space ()
{
  return buffer_space (&led_data);
}
