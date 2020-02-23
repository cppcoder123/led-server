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

#define SHIFT_DELAY 0

static volatile struct buffer_t shift_data;
static uint8_t stable_data[MIN_DATA_SIZE];

static struct display_t display_right;
static struct display_t display_left;

static uint8_t drain;

static volatile uint8_t brightness;

void flush_init ()
{
  /*fixme*/
  buffer_init (&shift_data);
  for (uint8_t i = 0; i < MIN_DATA_SIZE; ++i)
    stable_data[i] = 0;

  display_init (&display_left, PORTA0, PORTA2, PORTA4);
  display_init (&display_right, PORTA1, PORTA3, PORTA5);

  brightness = FLUSH_BRIGHTNESS_MAX;
  flush_brightness_set (brightness);

  drain = 0;
}

static uint8_t get_data (uint8_t shift_mode, uint8_t position)
{
  uint8_t byte;
  uint8_t status = 1;

  if (shift_mode != 0)
    status = buffer_byte_get (&shift_data, position, &byte);
  else
    byte = stable_data[position];

  return (status != 0) ? byte : 0;
}

static void dump (uint8_t shift_mode)
{
  display_data_start (&display_left);
  display_data_start (&display_right);

  for (uint8_t i = 0; i < MATRIX_SIZE; ++i) {
    display_data_column (&display_left, get_data (shift_mode, i));
    display_data_column (&display_right,
                         get_data (shift_mode, i + MATRIX_SIZE));
  }

  display_data_stop (&display_left);
  display_data_stop (&display_right);
}

uint8_t flush_shift_data (uint8_t *arr, uint8_t arr_size)
{
  if (drain != 0)
    return 1;

  return buffer_array_fill (&shift_data, arr, arr_size);
}

void flush_shift_display ()
{
  if (buffer_size (&shift_data) < MIN_DATA_SIZE)
    return;

  dump (1);

  /* shift */
  uint8_t symbol;
  buffer_byte_drain (&shift_data, &symbol);
}

void flush_shift_drain_start ()
{
  drain = 1;
  buffer_clear (&shift_data);
}

void flush_shift_drain_stop ()
{
  drain = 0;
}

uint8_t flush_shift_buffer_space ()
{
  return buffer_space (&shift_data);
}

uint8_t flush_shift_buffer_size ()
{
  return buffer_size (&shift_data);
}

void flush_stable_display (volatile struct buf_t *buf)
{
  uint8_t info = 0;
  for (uint8_t i = 0; i < MIN_DATA_SIZE; ++i)
    stable_data[i] = (buf_byte_get (buf, i, &info) != 0) ? info : 0;
  
  dump (0);
}

void flush_brightness_get (uint8_t *b_ness)
{
  *b_ness = brightness;
}

void flush_brightness_set (uint8_t b_ness)
{
  if (b_ness > FLUSH_BRIGHTNESS_MAX)
    return;

  brightness = b_ness;
  display_brightness (&display_left, brightness);
  display_brightness (&display_right, brightness);
}
