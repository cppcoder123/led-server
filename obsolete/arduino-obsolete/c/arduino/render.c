/*
 *
 */
#include <avr/interrupt.h>
#include <avr/io.h>

#include "device-id.h"

#include "matrix.h"
#include "parse.h"
#include "queue.h"
#include "render.h"
#include "spi-write.h"

#define TIMER_ENABLE TCCR0B = (1 << CS02) | (1 << CS00)
#define TIMER_DISABLE TCCR0B = 0

/* 64 * (10^-6) seconds*/
#define DELAY_TICK_MIN 255
/* ~16 * (10^-3) seconds*/
#define DELAY_TICK_MAX 180

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

/* SUB_MATRIX_SIZE ? */
static volatile uint8_t matrix_data_a[ID_MAX_BUFFER_SIZE];
static volatile uint8_t matrix_data_b[ID_MAX_BUFFER_SIZE];

static volatile struct queue_t matrix_queue_a;
static volatile struct queue_t matrix_queue_b;

static volatile struct matrix_t matrix_buffer_a;
static volatile struct matrix_t matrix_buffer_b;

static volatile struct matrix_t *matrix_a;
static volatile struct matrix_t *matrix_b;

static volatile struct matrix_t *parse_matrix;

static uint8_t index_left;      /* number of zeroes at the left */
static uint8_t index_a;

static volatile uint8_t delay_tick;
static volatile uint8_t delay_factor;


static void software_init_once ()
{
  pixel_delay = 6;
  phrase_delay = 12;
  stable_delay = 12;

  matrix_init (&matrix_buffer_a,
               &matrix_queue_a, matrix_data_a, ID_MAX_BUFFER_SIZE, 222);
  matrix_init (&matrix_buffer_b,
               &matrix_queue_b, matrix_data_b, ID_MAX_BUFFER_SIZE, 223);

  matrix_a = &matrix_buffer_a;
  matrix_b = &matrix_buffer_b;

  parse_matrix = parse_get_matrix ();
}

static void set_delay (uint8_t tick, uint8_t factor)
{
  delay_tick = tick;
  delay_factor = factor;
}

static void software_init ()
{
  state = STATE_EMPTY;

  index_left = index_a = 0;
  
  set_delay (DELAY_TICK_MAX, DELAY_FACTOR_EMPTY);
}

static void swap_matrices ()
{
  volatile struct matrix_t *tmp = matrix_a;
  matrix_a = matrix_b;
  matrix_b = tmp;
}

static void handle_empty ()
{
  if ((matrix_size (matrix_a) == 0)
      && (matrix_size (matrix_b) == 0)) {
    set_delay (DELAY_TICK_MAX, DELAY_FACTOR_EMPTY);
    return;
  }
  
  /* swap if needed */
  if ((matrix_size (matrix_a) == 0)
      && (matrix_size (matrix_b) != 0))
    swap_matrices ();

  state = (matrix_size (matrix_a) <= SPI_WRITE_MATRIX_SIZE)
    ? STATE_STABLE : STATE_PIXEL;
  
  if (state == STATE_PIXEL) {
    index_left = (matrix_a->type & ID_SUB_MATRIX_TYPE_FIRST)
      ? SPI_WRITE_MATRIX_SIZE : 0;

    index_a = 0;
  }

  set_delay (DELAY_TICK_MAX, DELAY_FACTOR_ZERO);
}

static void handle_stable ()
{
  uint8_t left_shift = (SPI_WRITE_MATRIX_SIZE - matrix_size (matrix_a)) / 2;

  volatile uint8_t *data;
  if (matrix_get (matrix_a, 0, &data) != 0)
    spi_write_matrix (data, left_shift, matrix_size (matrix_a));

  matrix_clear (matrix_a);

  state = STATE_RENDERED;
  
  set_delay (DELAY_TICK_MAX, stable_delay);
}

static uint8_t fill_buffer (uint8_t *data)
{
  uint8_t fill_size = SPI_WRITE_MATRIX_SIZE - index_left;

  // at least we have matrix_a
  uint8_t data_size = (fill_size <= (matrix_size (matrix_a) - index_a))
    ? fill_size : (matrix_size (matrix_a) - index_a);

  volatile uint8_t *src;
  if (matrix_get (matrix_a, index_a, &src) != 0)
    for (uint8_t i = 0; i < data_size; ++i)
      data[i] = src[i];

  if ((fill_size == data_size)  /* full */
      || (matrix_a->type & ID_SUB_MATRIX_TYPE_LAST) /* fill zeroes at the right */
      || (matrix_size (matrix_b) == 0))             /* no matrix-b */
    return data_size;

  uint8_t fill_b_size = fill_size - data_size;
  if (fill_b_size > matrix_size (matrix_b))
    fill_b_size = matrix_size (matrix_b);

  if (matrix_get (matrix_b, 0, &src) != 0)
    for (uint8_t j = data_size; j < fill_b_size + data_size; ++j)
      data[j] = src[j - data_size];
  
  return data_size + fill_b_size;
}

static uint8_t increment_index ()
{
  if (index_left > 0) {
    --index_left;
    return 1;
  }

  if (index_a < matrix_size (matrix_a)) {
    ++index_a;
    return 1;
  }

  /* index-a >= matrix_size (matrix_a) => drain & swap */
  uint8_t type = matrix_a->type;
  matrix_clear (matrix_a);
  if (type & ID_SUB_MATRIX_TYPE_LAST)
    return 0;

  swap_matrices ();
  index_a = 0;

  return matrix_size (matrix_a) != 0;
}

static void handle_pixel ()
{
  uint8_t buffer[SPI_WRITE_MATRIX_SIZE];
  uint8_t data_size = fill_buffer (buffer);

  spi_write_matrix (buffer, index_left, data_size);

  if (increment_index () == 0)
    state = STATE_RENDERED;
  
  set_delay (DELAY_TICK_MAX, (state == STATE_RENDERED) ? phrase_delay : pixel_delay);
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
    return;
  }

  TIMER_DISABLE;
  
  switch (state) {
  case STATE_EMPTY:
    handle_empty ();
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

  if (matrix_size (parse_matrix) != 0) {
    if (matrix_size (matrix_a) == 0)
      matrix_move (parse_matrix, matrix_a);
    else if (matrix_size (matrix_b) == 0)
      matrix_move (parse_matrix, matrix_b);
  }

  TCNT0 = delay_tick;
  TIMER_ENABLE;
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
