/*
 *
 */

#include <avr/io.h>

#include "unix/constant.h"

#include "buf.h"
#include "buffer.h"
#include "debug.h"
#include "display.h"
#include "encode.h"
#include "flush.h"
#include "invoke.h"

#define MATRIX_SIZE 32
#define MIN_DATA_SIZE (MATRIX_SIZE * 2)

#define SHIFT_DELAY 0

static volatile struct buffer_t shift_data;
static volatile struct buf_t stable_data;

static struct display_t display_right;
static struct display_t display_left;

enum {
  MODE_UNKNOWN,
  MODE_SHIFT,
  MODE_STABLE,
};
static uint8_t mode;

void flush_init ()
{
  /*fixme*/
  mode = MODE_UNKNOWN;

  buffer_init (&shift_data);
  buf_init (&stable_data);

  display_init (&display_left, PORTA0, PORTA2, PORTA4);
  display_init (&display_right, PORTA1, PORTA3, PORTA5);
}

static uint8_t get_data (uint8_t position)
{
  uint8_t byte;
  uint8_t status = (mode == MODE_SHIFT)
    ? buffer_byte_get (&shift_data, position, &byte)
    : buf_byte_get (&stable_data, position, &byte);

  return (status != 0) ? byte : 0;
}

static void dump ()
{
  if (mode == MODE_UNKNOWN)
    return;

  display_data_start (&display_left);
  display_data_start (&display_right);

  for (uint8_t i = 0; i < MATRIX_SIZE; ++i) {
    display_data_column (&display_left, get_data (i));
    display_data_column (&display_right, get_data (i + MATRIX_SIZE));
  }

  display_data_stop (&display_left);
  display_data_stop (&display_right);
}

uint8_t flush_push_array (uint8_t *arr, uint8_t arr_size)
{
  if (mode == MODE_UNKNOWN)
    return 0;

  return (mode == MODE_SHIFT)
    ? buffer_array_fill (&shift_data, arr, arr_size)
    : buf_array_fill (&stable_data, arr, arr_size);
}

static void dump_shift ()
{
  if (buffer_size (&shift_data) < MIN_DATA_SIZE)
    return;

  dump ();

  /* shift */
  uint8_t symbol;
  buffer_byte_drain (&shift_data, &symbol);
}

void flush_shift_enable ()
{
  mode = MODE_SHIFT;
  invoke_enable (INVOKE_ID_FLUSH, SHIFT_DELAY, &dump_shift);
}

void flush_shift_disable ()
{
  invoke_disable (INVOKE_ID_FLUSH);
  mode = MODE_UNKNOWN;
}

void flush_stable_enable ()
{
  invoke_disable (INVOKE_ID_FLUSH);
  mode = MODE_STABLE;
}

void flush_stable_disable ()
{
  mode = MODE_UNKNOWN;
}

void flush_stable_display ()
{
  dump ();

  /*clear*/
  buffer_clear (&shift_data);
}

uint8_t flush_buffer_space ()
{
  if (mode == MODE_UNKNOWN)
    return 0;

  return (mode == MODE_SHIFT)
    ? buffer_space (&shift_data) : buf_space (&stable_data);
}

uint8_t flush_buffer_size ()
{
  if (mode == MODE_UNKNOWN)
    return 0;

  return (mode == MODE_SHIFT)
    ? buffer_size (&shift_data) : buf_size (&stable_data);
}

void flush_buffer_clear ()
{
  if (mode == MODE_UNKNOWN)
    return;
  if (mode == MODE_SHIFT)
    buffer_clear (&shift_data);
  else
    buf_clear (&stable_data);
}
