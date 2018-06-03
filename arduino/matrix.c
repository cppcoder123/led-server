/*
 *
 */

#include "device-id.h"

#include "matrix.h"
#include "matrix-timer.h"
#include "spi-write.h"

#define PHYSICAL_MATRIX_SIZE SPI_WRITE_MATRIX_SIZE

volatile uint8_t matrix_data[ID_MAX_MATRIX_SIZE];
static uint16_t size;
static uint8_t state;

static uint8_t pixel_delay;
static uint8_t phrase_delay;
static uint8_t stable_delay;

volatile uint8_t *wait_condition;

enum {
  MATRIX_NEW = (1 << 0),
  MATRIX_RENDERED = (1 << 1)
};

void matrix_init ()
{
  wait_condition = matrix_timer_get_condition ();
  for (uint8_t i = 0; i < ID_MAX_MATRIX_SIZE; ++i)
    matrix_data[i] = 0;
  size = 0;
  state = 0;
  pixel_delay = 2;
  phrase_delay = 10;
  state = 0;
  wait_condition = matrix_timer_get_condition ();
}

static void shift_buffer (uint8_t *buffer, uint8_t info)
{
  for (uint8_t i = 0; i < PHYSICAL_MATRIX_SIZE - 1; ++i)
    buffer[i] = buffer[i + 1];

  buffer[PHYSICAL_MATRIX_SIZE - 1] = info;
}

static void render_shift ()
{
  uint8_t buffer[PHYSICAL_MATRIX_SIZE];
  for (uint8_t i = 0; i < PHYSICAL_MATRIX_SIZE; ++i)
    buffer[i] = 0;

  for (uint16_t j = 0; j < size + PHYSICAL_MATRIX_SIZE; ++j) {
    uint8_t info = (j < size) ? matrix_data[j] : 0;
    shift_buffer (buffer, info);
    spi_write_matrix (buffer);
    matrix_wait (pixel_delay);
  }

  matrix_wait (phrase_delay);

  state |= MATRIX_RENDERED;
}

static void render_stable ()
{
  spi_write_matrix (matrix_data);
  matrix_wait (stable_delay);
  state |= MATRIX_RENDERED;
}

static void render ()
{
  if (size > PHYSICAL_MATRIX_SIZE)
    render_shift ();
  else
    render_stable ();
}

/*returns zero in case of failure*/
uint8_t matrix_write_prepare (uint16_t new_size)
{
  if (new_size > ID_MAX_MATRIX_SIZE)
    return 0;

  if ((state != 0) && ((state & MATRIX_RENDERED) == 0))
    return 0;

  size = new_size;

  state &= (~MATRIX_NEW);
  state &= (~MATRIX_RENDERED);
  
  return 1;
}

void matrix_write (volatile uint8_t *new_data)
{
  uint16_t left = 0;

  if (size < PHYSICAL_MATRIX_SIZE) {
    /* left zeroes */
    left = (PHYSICAL_MATRIX_SIZE - size) / 2;
    for (uint16_t i = 0; i < left; ++i) {
      matrix_data[i] = 0;
    }
  }

  for (uint16_t i = left; i < size + left; ++i) {
    matrix_data[i] = new_data[i];
  }

  if (size < PHYSICAL_MATRIX_SIZE) {
    /* right zeroes */
    for (uint16_t i = left + size; i < PHYSICAL_MATRIX_SIZE; ++i) {
      matrix_data[i] = 0;
    }
  }

  if (size < PHYSICAL_MATRIX_SIZE)
    size = PHYSICAL_MATRIX_SIZE;

  state |= MATRIX_NEW;

  render ();
}

void matrix_shift_delay (uint8_t one_pixel_delay, uint8_t whole_phrase_delay)
{
  pixel_delay = one_pixel_delay;
  phrase_delay = whole_phrase_delay;
}

void matrix_stable_delay (uint8_t delay)
{
  stable_delay = delay;
}

void matrix_wait (uint8_t delay)
{
  for (uint8_t i = 0; i < delay; ++i) {
    *wait_condition = 1;
    while (*wait_condition)
      ;
  }
}
