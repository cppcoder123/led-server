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

volatile data_t read_buf[READ_SIZE];
volatile data_t write_buf[WRITE_SIZE];

static uint8_t interrupt_requested;

static void irq_start ()
{
  /*set irq pin 1*/
  if (interrupt_requested == 0) {
    PORTB |= (1 << SPI_IRQ);
    interrupt_requested = 1;
  }
}

static void irq_stop ()
{
  if (interrupt_requested != 0) {
    PORTB &= ~(1 << SPI_IRQ);
    interrupt_requested = 0;
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

  interrupt_requested = 1;
  irq_stop ();

  /*enable spi, and enable related interrupt*/
  SPCR = (1 << SPIE) | (1 << SPE);
}

uint8_t spi_read_symbol (data_t *symbol)
{
  return ring_symbol_drain (read_buf, symbol);
}

uint8_t spi_read_array (data_t *arr, uint8_t arr_size)
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

uint8_t spi_write_array (data_t *array, uint8_t array_size)
{
  irq_start ();
  return ring_array_fill (write_buf, array, array_size);
}

ISR (SPI_STC_vect)
{
  /*
   * 1. fill read buf with inbound symbol
   * 2. try to drain write buf and send it to master
   */

  /* fixme: debug */
  /* uint8_t tmp = SPDR; */
  /* SPDR = tmp; */
  /* return; */

  /* ring_init (write_buf, WRITE_SIZE); */

  if (ring_symbol_fill (read_buf, SPDR) == 0) {
    SPDR = SPI_READ_OVERFLOW;
    return;
  }

  data_t to_send;
  if (ring_symbol_drain (write_buf, &to_send) != 0) {
    SPDR = to_send;
  } else {
    irq_stop ();
    SPDR = SPI_WRITE_UNDERFLOW;
  }
}
