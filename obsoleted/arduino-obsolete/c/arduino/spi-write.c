/*
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#include "device-id.h"

/* #include "codec.h"              /\* debug *\/ */
#include "queue.h"
#include "spi-write.h"


#define SPI_SS (1 << PB2)
#define SPI_MOSI (1 << PB3)
#define SPI_SCK (1 << PB5)

#define SEND_COMMAND_MODE 1
#define SEND_DATA_MODE 2

#define LENGTH_COMMAND 2
/* 32 bytes for data, 2 for prefix */
#define LENGTH_DATA 34

#define CMD_SYSTEM_STOP 0x0
#define CMD_SYSTEM_START 0x1
#define CMD_NMOS_8COM 0x20
#define CMD_MASTER_MODE 0x18
#define CMD_LED_ON 0x3
#define CMD_BRIGHTNESS_PREFIX 0xA0

/* last 4 bits */
#define BRIGHTNESS_MASK 0xF

/* b100 shifted 5 times to the left */
#define CMD_PREFIX 0x80

/* 0b 1010 0000 */
#define DATA_PREFIX 0xA0
#define DATA_BUFFER_MASK 0x03

#define QUEUE_SIZE 50

static uint8_t queue_data[QUEUE_SIZE];
static volatile struct queue_t queue;

static volatile uint8_t send_count;
static uint8_t send_size;

static void reset_send_info ()
{
  send_count = 0;
  send_size = 0;
}

void spi_write_init ()
{
  queue_init (&queue, queue_data, QUEUE_SIZE, 3);
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
  SPCR = (1 << SPIE) | (1 << SPE) | (1 << MSTR)
    | (1 << CPOL) | (1 << CPHA)
    | (1 << SPR1) | (1 << SPR0);
}

static void slave_select ()
{
  /* active level is low, so */
  PORTB &= ~SPI_SS;
  /* codec_encode_1 (ID_STATUS, ID_DEVICE_SERIAL, 200); */
}

static void slave_deselect ()
{
  PORTB |= SPI_SS;
  /* codec_encode_1 (ID_STATUS, ID_DEVICE_SERIAL, 100); */
}

ISR (SPI_STC_vect)
{
  if (send_count == 0) {
    if (queue_drain_symbol (&queue, &send_size) == 0)
      return;
    slave_select ();
  }

  if (send_count >= send_size) {
    slave_deselect ();
    reset_send_info ();
    return;
  }

  uint8_t data;
  if (queue_drain_symbol (&queue, &data) != 0) {
    ++send_count;
    SPDR = data;
  } else {
    /* we should not get here */
    /* codec_encode_1 (ID_STATUS, ID_DEVICE_SERIAL, 201); */
    slave_deselect ();
  }
}

static void start_send ()
{
  if ((send_count > 0)          /* send in progress */
      || (queue_is_drainable (&queue, 1) == 0)) /* empty queue */
    return;

  /* dummy data to call interrupt */
  SPDR = 0;
}

static void fill_command (uint8_t cmd)
{
  queue_fill_symbol (&queue, LENGTH_COMMAND);
  /* 100 for command prefix */
  queue_fill_symbol (&queue, CMD_PREFIX | (cmd >> 3));
  queue_fill_symbol (&queue, (cmd << 5));

  start_send ();
}

void spi_write_initialize ()
{
  fill_command (CMD_SYSTEM_STOP);
  fill_command (CMD_NMOS_8COM);
  fill_command (CMD_MASTER_MODE); /* ??? */
  fill_command (CMD_SYSTEM_START);
  fill_command (CMD_LED_ON);
  fill_command (CMD_BRIGHTNESS_PREFIX | (BRIGHTNESS_MASK & ID_BRIGHTNESS_MAX));
  fill_command (CMD_BRIGHTNESS_PREFIX | (BRIGHTNESS_MASK & ID_BRIGHTNESS_MAX));
}

void spi_write_uninitialize ()
{
  fill_command (CMD_SYSTEM_STOP);
}

void spi_write_brightness (uint8_t brightness)
{
  /* 
   * Brightness levels are the same as in device-id, 
   * so no conversion is needed
   */
  fill_command (CMD_BRIGHTNESS_PREFIX | (BRIGHTNESS_MASK & brightness));
}

static uint8_t transpose (uint8_t src)
{
  uint8_t low = src & 0xF;
  uint8_t high = src & 0xF0;

  uint8_t result = (low & 0x1);
  result <<= 1;
  result |= (low >>= 1) & 0x1;
  result <<= 1;
  result |= (low >>= 1) & 0x1;
  result <<= 1;
  result |= (low >>= 1) & 0x1;
  result <<= 1;

  high >>= 4;

  result |= (high & 0x1);
  result <<= 1;
  result |= (high >>= 1) & 0x1;
  result <<= 1;
  result |= (high >>= 1) & 0x1;
  result <<= 1;
  result |= (high >>= 1) & 0x1;

  return result;
}

static uint8_t fill_data (uint8_t data, uint8_t buf)
{
  /* keep last 2 bits of data */
  uint8_t tmp = data & DATA_BUFFER_MASK;
  /* shift data right 2 times and OR with buffer*/
  queue_fill_symbol (&queue, buf | (data >> 2));
  /* shift data we want to save to the left */
  return (tmp << 6);
}

static uint8_t get_data (volatile uint8_t *data, uint8_t left_empty,
                         uint8_t data_size, uint8_t index)
{
  if (index < left_empty)
    return 0;

  uint8_t shift = index - left_empty;
  if (shift < data_size)
    return data[shift];

  return 0;
}

void spi_write_matrix (volatile uint8_t *data, uint8_t left_empty, uint8_t data_size)
{
  queue_fill_symbol (&queue, LENGTH_DATA);
  /*101 | 00000 : cmd code + 5 bits of zero address*/
  queue_fill_symbol (&queue, DATA_PREFIX);

  uint8_t bits_buf = 0;         /* keep 2 bits from prev data here */
  uint8_t first_symbol = get_data (data, left_empty, data_size, 0);
  uint8_t bits_first = (transpose (first_symbol) >> 2);

  for (uint8_t i = 0; i < SPI_WRITE_MATRIX_SIZE; ++i) {
    uint8_t symbol = get_data (data, left_empty, data_size, i);
    bits_buf = fill_data (transpose (symbol), bits_buf);
  }

  /* codec_encode_1 (ID_STATUS, ID_DEVICE_SERIAL, bits_buf | bits_first); */
  queue_fill_symbol (&queue, bits_buf | bits_first);
  
  start_send ();
}

void spi_write_matrix_test (uint8_t pattern, uint8_t start, uint8_t size)
{
  uint8_t buf[SPI_WRITE_MATRIX_SIZE];
  for (uint8_t i = 0; i < SPI_WRITE_MATRIX_SIZE; ++i)
    buf[i] = pattern;

  spi_write_matrix (buf, start, size);
}
