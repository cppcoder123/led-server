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
#include "invoke.h"

#define MATRIX_SIZE 32
#define MIN_DATA_SIZE (MATRIX_SIZE * 2)

#define SHIFT_DELAY 0

static volatile struct buffer_t led_data;

static struct display_t display_right;
static struct display_t display_left;

void flush_init ()
{
  /*fixme*/
  buffer_init (&led_data);

  display_init (&display_left, PORTA0, PORTA2, PORTA4);
  display_init (&display_right, PORTA1, PORTA3, PORTA5);
}

static void dump ()
{
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
}

uint8_t flush_push (uint8_t symbol)
{
  return buffer_byte_fill (&led_data, symbol);
}

uint8_t flush_push_array (uint8_t *arr, uint8_t arr_size)
{
  return buffer_array_fill (&led_data, arr, arr_size);
}

static void dump_shift ()
{
  if (buffer_size (&led_data) < MIN_DATA_SIZE)
    return;

  dump ();

  /* shift */
  uint8_t symbol;
  buffer_byte_drain (&led_data, &symbol);
}

void flush_shift_enable ()
{
  invoke_enable (INVOKE_ID_FLUSH, SHIFT_DELAY, &dump_shift);
}

void flush_shift_disable ()
{
  invoke_disable (INVOKE_ID_FLUSH);
}

void flush_dump_clear ()
{
  dump ();

  /*clear*/
  buffer_clear (&led_data);
}

uint8_t flush_buffer_space ()
{
  return buffer_space (&led_data);
}

uint8_t flush_buffer_size ()
{
  return buffer_size (&led_data);
}

void flush_buffer_clear ()
{
  buffer_clear (&led_data);
}
