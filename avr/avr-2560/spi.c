/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "mcu/constant.h"

#include "debug.h"
#include "ring.h"
#include "spi.h"

#define SPI_MISO PORTB3
/* irq pin is connected to pi's gpio-26 */
#define SPI_IRQ PORTB4

#define READ_SIZE 255
#define WRITE_SIZE 255

#define FLAG_WRITE_INTERRUPT (1 << 0)

volatile uint8_t read_buf[READ_SIZE];
volatile uint8_t write_buf[WRITE_SIZE];

volatile uint8_t flag;

static void write_interrupt_start ()
{
  /*set irq pin 1*/
  if ((flag & FLAG_WRITE_INTERRUPT) == 0) {
    PORTB |= (1 << SPI_IRQ);
    flag |= FLAG_WRITE_INTERRUPT;
  }
}

static void write_interrupt_stop ()
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

  ring_init (read_buf, READ_SIZE);
  ring_init (write_buf, WRITE_SIZE);

  flag = FLAG_WRITE_INTERRUPT;
  write_interrupt_stop ();

  /*enable spi, and enable related interrupt*/
  SPCR = (1 << SPIE) | (1 << SPE);
}

uint8_t spi_read_symbol (uint8_t *symbol)
{
  return ring_symbol_drain (read_buf, symbol);
}

uint8_t spi_read_array (uint8_t *arr, uint8_t arr_size)
{
  return ring_array_drain (read_buf, arr, arr_size);
}

uint8_t spi_read_space ()
{
  return ring_space (read_buf);
}

uint8_t spi_read_size ()
{
  return ring_size (read_buf);
}

uint8_t spi_write_array (uint8_t *array, uint8_t array_size)
{
  uint8_t status = ring_array_fill (write_buf, array, array_size);
  write_interrupt_start ();
  return status;
}

ISR (SPI_STC_vect)
{
  /*
   * 1. fill read buf with inbound symbol
   * 2. try to drain write buf and send it to master
   */

  if (ring_symbol_fill (read_buf, SPDR) == 0) {
    SPDR = SPI_READ_OVERFLOW;
    return;
  }

  /*long interrupt handling?*/
  /* SPDR = SPI_WRITE_UNDERFLOW; */

  uint8_t to_send;
  if (ring_symbol_drain (write_buf, &to_send) != 0) {
    SPDR = to_send;
  } else {
    SPDR = SPI_WRITE_UNDERFLOW;
    write_interrupt_stop ();
  }
}
