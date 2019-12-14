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
#define POLL_LIMIT 64 /* ? */

static volatile struct buffer_t led_data;

static struct display_t display_1;

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

  display_init (&display_1, PORTA2, PORTA6, PORTA4);
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
      || (buffer_size (&led_data) < MATRIX_SIZE))
    return;

  /* if (buffer_size (&led_data) < POLL_LIMIT) */
  /*   encode_msg_1 (MSG_ID_POLL, SERIAL_ID_TO_IGNORE, flush_buffer_space ()); */

  display_data_start (&display_1);

  uint8_t symbol;
  for (uint8_t i = 0; i < MATRIX_SIZE; ++i)
    if (buffer_byte_get (&led_data, i, &symbol) != 0)
      display_data_column (&display_1, symbol);
    else
      display_data_column (&display_1, 0);

  display_data_stop (&display_1);

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
