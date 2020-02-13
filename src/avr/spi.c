/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "unix/constant.h"

#include "buffer.h"
#include "spi.h"

#define SPI_MISO PORTB3
/* irq pin is connected to pi's gpio-27 */
#define SPI_IRQ PORTB6

#define FLAG_WRITE_INTERRUPT (1 << 0)

static volatile struct buffer_t read_buf;
static volatile struct buffer_t write_buf;

static volatile uint8_t flag;

void spi_interrupt_start ()
{
  /*set irq pin 1*/
  if ((flag & FLAG_WRITE_INTERRUPT) == 0) {
    PORTB |= (1 << SPI_IRQ);
    flag |= FLAG_WRITE_INTERRUPT;
  }
}

static void interrupt_stop ()
{
  if ((flag & FLAG_WRITE_INTERRUPT) != 0) {
    PORTB &= ~(1 << SPI_IRQ);
    flag &= ~FLAG_WRITE_INTERRUPT;
  }
}

void spi_init ()
{
  /*
   * Configure spi,
   * MISO is output, all others are inputs
   */
  DDRB |= (1 << SPI_MISO) | (1 << SPI_IRQ);

  /*clear*/
  SPDR = 0;

  buffer_init (&read_buf);
  buffer_init (&write_buf);

  flag = FLAG_WRITE_INTERRUPT;
  interrupt_stop ();

  /*enable spi and enable related interrupt*/
  SPCR = (1 << SPIE) | (1 << SPE);
}

uint8_t spi_read_symbol (uint8_t *symbol)
{
  return buffer_byte_drain (&read_buf, symbol);
}

uint8_t spi_read_array (uint8_t *arr, uint8_t arr_size)
{
  return buffer_array_drain (&read_buf, arr, arr_size);
}

uint8_t spi_read_space ()
{
  return buffer_space (&read_buf);
}

uint8_t spi_write_space ()
{
  return buffer_space (&write_buf);
}

uint8_t spi_read_size ()
{
  return buffer_size (&read_buf);
}

uint8_t spi_write_array (uint8_t *array, uint8_t array_size)
{
  uint8_t status = buffer_array_fill (&write_buf, array, array_size);
  spi_interrupt_start ();
  return status;
}

ISR (SPI_STC_vect)
{
  /*
   * 1. fill read buf with inbound symbol
   * 2. try to drain write buf and send it to master
   */

  if (buffer_byte_fill (&read_buf, SPDR) == 0) {
    SPDR = SPI_READ_OVERFLOW;
    return;
  }

  uint8_t to_send;
  if (buffer_byte_drain (&write_buf, &to_send) != 0) {
    SPDR = to_send;
  } else {
    SPDR = SPI_WRITE_UNDERFLOW;
    interrupt_stop ();
  }
}
