/*
 *
 */

#include "device-id.h"

#include "matrix.h"
#include "matrix-timer.h"
#include "spi-write.h"

#define PHYSICAL_MATRIX_SIZE SPI_WRITE_MATRIX_SIZE

static uint8_t matrix_data[ID_MAX_MATRIX_SIZE];
static uint16_t size;
static uint8_t state;

static uint8_t pixel_delay;
static uint8_t phrase_delay;
static uint8_t stable_delay;

enum {
  MATRIX_UPDATE_STARTED = (1 << 0),
  MATRIX_UPDATE_FINISHED = (1 << 1),
  MATRIX_RENDERED = (1 << 2)
};

void matrix_init ()
{
  for (uint8_t i = 0; i < ID_MAX_MATRIX_SIZE; ++i)
    matrix_data[i] = 0;
  size = 0;
  state = 0;
  pixel_delay = 2;
  phrase_delay = 10;
  state = 0;
}

static void buffer_left_shift (uint8_t *buffer, uint8_t step, uint8_t fill_pattern)
{
  for (uint8_t i = 0; i < PHYSICAL_MATRIX_SIZE - step; ++i)
    buffer[i] = buffer[i + step];

  for (uint8_t j = PHYSICAL_MATRIX_SIZE - step; j < PHYSICAL_MATRIX_SIZE; ++j)
    buffer[j] = fill_pattern;
}

static void buffer_right_shift (uint8_t *buffer, uint8_t step, uint8_t fill_pattern)
{
  for (uint8_t i = PHYSICAL_MATRIX_SIZE - step - 1; i >= 0; --i)
    buffer[i + step] = buffer[i];
  for (uint8_t j = 0; j < step; ++j)
    buffer[j] = fill_pattern;
}

static void center_data ()
{
  if (size >= PHYSICAL_MATRIX_SIZE)
    return;

  uint8_t left = (PHYSICAL_MATRIX_SIZE - (uint8_t)size) / 2;

  buffer_right_shift (matrix_data, left, 0);
  size += left;

  for (uint8_t i = size; i < PHYSICAL_MATRIX_SIZE; ++i)
    matrix_data[i] = 0;

  size = PHYSICAL_MATRIX_SIZE;
}

static void render_shift ()
{
  uint8_t buffer[PHYSICAL_MATRIX_SIZE];
  for (uint8_t i = 0; i < PHYSICAL_MATRIX_SIZE; ++i)
    buffer[i] = 0;

  for (uint16_t j = 0; j < size + PHYSICAL_MATRIX_SIZE; ++j) {
    uint8_t info = (j < size) ? matrix_data[j] : 0;
    buffer_left_shift (buffer, 1, info);
    spi_write_matrix (buffer);
    matrix_timer_wait (pixel_delay);
  }

  matrix_timer_wait (phrase_delay);

  state |= MATRIX_RENDERED;
}

static void render_stable ()
{
  spi_write_matrix (matrix_data);
  matrix_timer_wait (stable_delay);
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
uint8_t matrix_update_start (uint8_t type)
{
  if (type & ID_SUB_MATRIX_TYPE_FIRST) {
    state = MATRIX_UPDATE_STARTED;
    size = 0;
    return 1;
  }

  if ((type & ID_SUB_MATRIX_TYPE_MASK) == 0)
    /* bad value */
    return 0;

  return 1;
}

uint8_t matrix_update (volatile uint8_t *update_data, uint8_t update_size)
{
  /* check size first */
  if (size + update_size > ID_MAX_MATRIX_SIZE)
    return 0;

  for (uint8_t i = 0; i < update_size; ++i) {
    matrix_data[size + i] = update_data[i];
  }

  size += update_size;

  return 1;
}

uint8_t matrix_update_finish (uint8_t type)
{
  if ((type & ID_SUB_MATRIX_TYPE_LAST) == 0)
    /* everything is OK, do nothing */
    return 1;

  /* Do we need to display empty matrix */
  
  center_data ();

  state |= MATRIX_UPDATE_FINISHED;

  render ();

  return 1;
}

void matrix_pixel_delay (uint8_t one_pixel_delay)
{
  pixel_delay = one_pixel_delay;
}

void matrix_phrase_delay (uint8_t whole_phrase_delay)
{
  phrase_delay = whole_phrase_delay;
}

void matrix_stable_delay (uint8_t delay)
{
  stable_delay = delay;
}
