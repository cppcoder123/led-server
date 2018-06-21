/*
 *
 */
#include <avr/interrupt.h>
#include <avr/io.h>

#include "device-id.h"

#include "matrix-buffer.h"
#include "render.h"
#include "spi-write.h"

/* 64 * (10^-6) seconds*/
#define DELAY_TICK_MIN 255
/* ~16 * (10^-3) seconds*/
#define DELAY_TICK_MAX 0

/* ~0.1 seconds with TICK_MAX */
#define DELAY_FACTOR_EMPTY 6
/* no factor */
#define DELAY_FACTOR_ZERO 0

enum {
  STATE_EMPTY,               /* next state either stable or rolling */
  STATE_STABLE,              /* stable render */
  STATE_PIXEL,               /* rolling render, loop here */
  /* STATE_PHRASE,              /\* end of rolling *\/ */
  STATE_RENDERED             /* next state is zero */
};


static volatile uint8_t state;

static uint8_t pixel_delay;
static uint8_t phrase_delay;
static uint8_t stable_delay;

static volatile uint8_t buffer[ID_MAX_MATRIX_SIZE];
static volatile uint16_t buffer_size;
static uint16_t buffer_shift;

static volatile uint8_t delay_tick;
static volatile uint8_t delay_factor;

static void set_delay (uint8_t tick, uint8_t factor)
{
  delay_tick = tick;
  delay_factor = factor;
}

static void software_init ()
{
  state = STATE_EMPTY;

  buffer_size = 0;
  buffer_shift = 0;

  set_delay (DELAY_TICK_MAX, DELAY_FACTOR_EMPTY);
}

static void right_shift (uint8_t step, uint8_t fill_pattern)
{
  for (uint8_t i = SPI_WRITE_MATRIX_SIZE - step - 1; i >= 0; --i)
    buffer[i + step] = buffer[i];
  for (uint8_t j = 0; j < step; ++j)
    buffer[j] = fill_pattern;
}

static void center_data ()
{
  if (buffer_size >= SPI_WRITE_MATRIX_SIZE)
    return;

  uint8_t left = (SPI_WRITE_MATRIX_SIZE - (uint8_t)buffer_size) / 2;

  right_shift (left, 0);
  buffer_size += left;

  for (uint8_t i = buffer_size; i < SPI_WRITE_MATRIX_SIZE; ++i)
    buffer[i] = 0;

  buffer_size = SPI_WRITE_MATRIX_SIZE;
}

static void handle_zero ()
{
  if (matrix_buffer_drain (&buffer, &buffer_size) == 0) {
    set_delay (DELAY_TICK_MAX, DELAY_FACTOR_EMPTY);
    return;
  }

  state = (buffer_size < SPI_WRITE_MATRIX_SIZE)
    ? STATE_STABLE : STATE_PIXEL;

  center_data ();

  set_delay (DELAY_TICK_MIN, DELAY_FACTOR_ZERO);
}

static void handle_stable ()
{
  spi_write_matrix (buffer, 0, SPI_WRITE_MATRIX_SIZE);

  state = STATE_RENDERED;
  
  set_delay (DELAY_TICK_MAX, stable_delay);
}

static void handle_pixel ()
{
  uint8_t left_empty
    = (buffer_shift < SPI_WRITE_MATRIX_SIZE) ? buffer_shift : 0;
  uint8_t data_size
    = ((buffer_shift + SPI_WRITE_MATRIX_SIZE) < buffer_size)
    ? SPI_WRITE_MATRIX_SIZE : (uint8_t) (buffer_size - buffer_shift);

  spi_write_matrix (buffer + buffer_shift, left_empty, data_size);
  
  /* int16_t loop_end = buffer_shift + SPI_WRITE_MATRIX_SIZE; */
  /* uint8_t data_size = (loop_end <= buffer_size) */

  

  /* for (int16_t i = buffer_shift; i < loop_end; ++i) { */
  /*   uint8_t type = (i == buffer_shift) ? SPI_WRITE_FIRST */
  /*     : (i == (loop_end - 1)) ? SPI_WRITE_LAST : SPI_WRITE_MIDDLE; */
  /*   uint8_t data = ((i >= 0) && (i < buffer_size)) ? buffer[i] : 0; */
  /*   spi_write_matrix_symbol (type, data); */
  /* } */

  ++buffer_shift;

  if (buffer_shift >= buffer_size) {
    state = STATE_RENDERED;
    set_delay (DELAY_TICK_MAX, phrase_delay);
    return;
  }
  
  set_delay (DELAY_TICK_MAX, pixel_delay);
}

static void handle_rendered ()
{
  /* it sets state and delay and resets everything */
  software_init ();
}

ISR (TIMER0_OVF_vect)
{
  if (delay_factor > 0) {
    --delay_factor;
    TCNT0 = delay_tick;
  }

  switch (state) {
  case STATE_EMPTY:
    handle_zero ();
    break;
  case STATE_STABLE:
    handle_stable ();
    break;
  case STATE_PIXEL:
    handle_pixel ();
    break;
  case STATE_RENDERED:
    handle_rendered ();
    break;
  default:
    break;
  }

  TCNT0 = delay_tick;
}

void render_init ()
{
  /*
   * Though default mode is increment,
   * we need to set it to zero to clear it after arduino loader
   */
  TCCR0A = 0;

  /*
   * Max clock prescaler = 1024
   */
  TCCR0B = (1 << CS02) | (1 << CS00);

  /* 
   * Clear interrupt source,
   * Enable overflow interrupt
   */
  TIFR0 |= (1 << TOV0);
  TIMSK0 |= (1 << TOIE0);

  TCNT0 = DELAY_TICK_MAX;

  /* init delays here because we want to use software_init from other place */
  pixel_delay = 6;
  phrase_delay = 12;
  stable_delay = 12;

  software_init ();
}

void render_pixel_delay (uint8_t delay)
{
  pixel_delay = delay;
}

void render_phrase_delay (uint8_t delay)
{
  phrase_delay = delay;
}

void render_stable_delay (uint8_t delay)
{
  stable_delay = delay;
}
