/*
 *
 */
#include <avr/interrupt.h>
#include <avr/io.h>

#include "device-id.h"

#include "matrix-buffer.h"
#include "queue.h"
#include "render.h"
#include "spi-write.h"

#define TIMER_ENABLE TCCR0B = (1 << CS02) | (1 << CS00)
#define TIMER_DISABLE TCCR0B = 0

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
  STATE_RENDERED             /* next state is zero */
};


static volatile uint8_t state;

static uint8_t pixel_delay;
static uint8_t phrase_delay;
static uint8_t stable_delay;

static volatile uint8_t buffer_data[ID_MAX_BUFFER_SIZE];
static volatile struct queue_t buffer;

static volatile uint8_t data_type;

static uint8_t buffer_shift;
static uint8_t buffer_shift_max;

static volatile uint8_t delay_tick;
static volatile uint8_t delay_factor;


static void software_init_once ()
{
  pixel_delay = 6;
  phrase_delay = 12;
  stable_delay = 12;

  queue_init (&buffer, buffer_data, ID_MAX_BUFFER_SIZE, 0xCC);
}

static void set_delay (uint8_t tick, uint8_t factor)
{
  delay_tick = tick;
  delay_factor = factor;
}

static void software_init ()
{
  state = STATE_EMPTY;

  queue_clear (&buffer);
  buffer_shift = 0;
  buffer_shift_max = 0;

  set_delay (DELAY_TICK_MAX, DELAY_FACTOR_EMPTY);
}

static void handle_zero ()
{
  if (matrix_buffer_drain (&data_type, &buffer) == 0) {
    set_delay (DELAY_TICK_MAX, DELAY_FACTOR_EMPTY);
    return;
  }

  state = (queue_size (&buffer) <= SPI_WRITE_MATRIX_SIZE)
    ? STATE_STABLE : STATE_PIXEL;

  if (state == STATE_PIXEL) {
    buffer_shift = (data_type & ID_SUB_MATRIX_TYPE_FIRST)
      ? 0 : SPI_WRITE_MATRIX_SIZE;

    buffer_shift_max = queue_size (&buffer);
    if (data_type & ID_SUB_MATRIX_TYPE_LAST)
      buffer_shift_max += SPI_WRITE_MATRIX_SIZE;
  }

  set_delay (DELAY_TICK_MAX, DELAY_FACTOR_ZERO);
}

static void handle_stable ()
{
  TIMER_DISABLE;
  uint8_t left_shift = (SPI_WRITE_MATRIX_SIZE - queue_size (&buffer)) / 2;
  volatile uint8_t *data;

  if (queue_get (&buffer, 0, &data) != 0)
    spi_write_matrix (data, left_shift, queue_size (&buffer));

  queue_clear (&buffer);

  state = STATE_RENDERED;
  
  set_delay (DELAY_TICK_MAX, stable_delay);
  TIMER_ENABLE;
}

static void handle_pixel ()
{
  // NOTE: Enable timer at return
  TIMER_DISABLE;

  uint8_t left_empty = (buffer_shift < SPI_WRITE_MATRIX_SIZE)
    ? (SPI_WRITE_MATRIX_SIZE - buffer_shift) : 0;

  uint16_t data_start = (buffer_shift < SPI_WRITE_MATRIX_SIZE)
    ? 0 : (buffer_shift - SPI_WRITE_MATRIX_SIZE);

  if (buffer_shift < buffer_shift_max) {

    /* spi_write_matrix reads only 32 symbols */
    uint8_t data_size
      = ((data_start + SPI_WRITE_MATRIX_SIZE) < queue_size (&buffer))
      ? SPI_WRITE_MATRIX_SIZE : (queue_size (&buffer) - data_start);

    volatile uint8_t *data;
    if (queue_get (&buffer, data_start, &data) != 0)
      spi_write_matrix (data, left_empty, data_size);
  
    ++buffer_shift;
  } else {
    state = STATE_RENDERED;
  }

  uint8_t long_delay = ((state == STATE_RENDERED)
                        && (data_type & ID_SUB_MATRIX_TYPE_LAST)) ? 1 : 0;
  set_delay (DELAY_TICK_MAX, (long_delay != 0) ? phrase_delay : pixel_delay);
  
  TIMER_ENABLE;
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
  /* TCCR0B = (1 << CS02) | (1 << CS00); */
  TIMER_ENABLE;

  /* 
   * Clear interrupt source,
   * Enable overflow interrupt
   */
  TIFR0 |= (1 << TOV0);
  TIMSK0 |= (1 << TOIE0);

  TCNT0 = DELAY_TICK_MAX;

  software_init_once ();
  
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
