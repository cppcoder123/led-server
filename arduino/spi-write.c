/*
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#include "device-id.h"

#include "queue.h"
#include "spi-write.h"

#define QUEUE_SIZE 48

#define SPI_SS (1 << PB2)
#define SPI_MOSI (1 << PB3)
#define SPI_MISO (1 << PB4)
#define SPI_SCK (1 << PB5)

#define SEND_COMMAND_MODE 1
#define SEND_DATA_MODE 2

#define LENGTH_COMMAND 2
/* 32 bytes for data, 2 for prefix */
#define LENGTH_DATA 34

#define CMD_SYSTEM_STOP 0x0
#define CMD_SYSTEM_START 0x1
#define CMD_NMOS_8COM 0x20
#define CMD_LED_ON 0x3
#define CMD_BRIGHTNESS_PREFIX 0xA0

/* last 4 bits */
#define BRIGHTNESS_MASK 0xF

/* b100 shifted 5 times to the left */
#define CMD_PREFIX 0x80

#define DATA_MASK 0x1

static uint8_t queue_data[QUEUE_SIZE];
static volatile struct queue_t queue;

static volatile uint8_t send_mode;
static volatile uint8_t send_count;

static void reset_send_info ()
{
  send_mode = 0;
  send_count = 0;
}

void spi_write_init ()
{
  queue_init (&queue, queue_data, QUEUE_SIZE, 0);
  reset_send_info ();

  /* 
   * Configure SPI port directions,
   * ss - output, mosi -output, miso - input, sck -output
   */
  DDRB = SPI_SS | SPI_MOSI | SPI_SCK;

  /*
   * Clear
   */
  SPDR = 0;

  /*
   * Enable SPI, enable interrupt, select master mode,
   *  fixme: do we need to select clock polarity and phase?
   *  Select clock rate (fixme: should we use slowest?)
   *  SPR01 & SPR00 => f-osc / 128
   *  0 for MSB first
   */
  SPCR = (1 << SPIE) | (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}

static void activate_ss ()
{
  /* active level is low, so */
  PORTB &= ~SPI_SS;
}

static void deactivate_ss ()
{
  PORTB |= SPI_SS;
}

static void select_mode (uint8_t mode)
{
  if ((mode == SEND_COMMAND_MODE)
      || (mode == SEND_DATA_MODE))
    queue_fill_symbol (&queue, mode);
}

static void start_send ()
{
  if ((send_mode != 0)          /* send in progress */
      || (queue_is_drainable (&queue, 1) == 0)) /* empty queue */
    return;

  /* dummy data to call interrupt */
  SPDR = 0;
}

static void fill_command (uint8_t cmd)
{
  queue_fill_symbol (&queue, CMD_PREFIX | (cmd >> 3));
  queue_fill_symbol (&queue, (cmd << 5));
}

static void fill_data_prefix ()
{
  /*10 - left 2 bits for 101*/
  queue_fill_symbol (&queue, (1 << 1));
  /* right bit of 101 and zero address*/
  queue_fill_symbol (&queue, (1 << 7));
}

static void fill_data (uint8_t data)
{
  /* write bits in reverse order */
  uint8_t result = 0;

  for (uint8_t i = 0; i < 8; ++i) {
    result <<= 1;       /* zero shift gives zero for first iteration*/
    result |= data & DATA_MASK;
    data >>= 1;
  }

  queue_fill_symbol (&queue, result);
}

ISR (SPI_STC_vect)
{
  if ((send_count == 0) && (send_mode == 0)) {
    uint8_t mode;
    if (queue_drain_symbol (&queue, &mode) == 0)
      return;
    if ((mode != SEND_COMMAND_MODE)
        || (mode != SEND_DATA_MODE))
      return;
    send_mode = mode;
    activate_ss ();
  } else {
    /* we are not changing mode => increase count */
    ++send_count;
  }

  if (((send_mode == SEND_COMMAND_MODE) && (send_count == LENGTH_COMMAND))
      || ((send_mode == SEND_DATA_MODE) && (send_count == LENGTH_DATA))) {
    deactivate_ss ();
    reset_send_info ();
    SPDR = 0;                   /* dummy data to call interrupt */
    return;
  }

  uint8_t data;
  if (queue_drain_symbol (&queue, &data) != 0)
    SPDR = data;
}

void spi_write_initialize ()
{
  select_mode (SEND_COMMAND_MODE);

  fill_command (CMD_SYSTEM_STOP);
  fill_command (CMD_NMOS_8COM);
  fill_command (CMD_SYSTEM_START);
  fill_command (CMD_LED_ON);
  fill_command (CMD_BRIGHTNESS_PREFIX | (BRIGHTNESS_MASK & ID_BRIGHTNESS_MAX));

  start_send ();
}

void spi_write_uninitialize ()
{
  select_mode (SEND_COMMAND_MODE);

  fill_command (CMD_SYSTEM_STOP);

  start_send ();
}

void spi_write_brightness (uint8_t brightness)
{
  select_mode (SEND_COMMAND_MODE);

  /* 
   * Brightness levels are the same as in device-id, 
   * so no conversion is needed
   */
  fill_command (CMD_BRIGHTNESS_PREFIX | (BRIGHTNESS_MASK & brightness));

  start_send ();
}

void spi_write_matrix_symbol (uint8_t type, uint8_t symbol)
{
  if (type == SPI_WRITE_FIRST) {
    select_mode (SEND_DATA_MODE);
    fill_data_prefix ();
  }

  fill_data (symbol);

  if (type == SPI_WRITE_LAST)
    start_send ();
}

