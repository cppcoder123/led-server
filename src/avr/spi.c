/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "unix/constant.h"

#include "buffer.h"
#include "spi.h"

#define CHANNEL_MISO PORTB3

/*
 * irq pin is connected to pi's gpio-27
 * This is an artificial interrupt to notify pi that
 * avr wants to write smth into the channel
 */
#define CHANNEL_IRQ PORTB4

/*
 * Tell pi that avr is ready
 */
#define CHANNEL_CONFIRMATION PORTB5

/* 3rd bit of port D tracks pi spi enable signal */
#define SHIFTER_NOTIFY PIND3

static struct buffer_t read_buf;
static struct buffer_t write_buf;

void spi_interrupt_start ()
{
  /*set irq pin 1*/
  PORTB |= (1 << CHANNEL_IRQ);
}

static void interrupt_stop ()
{
  PORTB &= ~(1 << CHANNEL_IRQ);
}

static void channel_enable ()
{
  buffer_clear (&read_buf);
  buffer_clear (&write_buf);

  /* send confirmation to pi */
  PORTB |= (1 << CHANNEL_CONFIRMATION);
}

static void channel_disable ()
{
  /* set confirmation signal to low*/
  PORTB &= ~(1 << CHANNEL_CONFIRMATION);
}

void spi_init ()
{
  /*
   * Configure spi output signals
   */
  DDRB |= (1 << CHANNEL_MISO)
    | (1 << CHANNEL_IRQ) | (1 << CHANNEL_CONFIRMATION);

  /*clear*/
  SPDR = 0;

  buffer_init (&read_buf);
  buffer_init (&write_buf);

  /* keep channel disabled for now */
  channel_disable ();

  /* attach interrupt to channel enable/disable */
  EICRA |= (1 << ISC30);        /* both edges should generate interrupt */
  EIMSK |= (1 << INT3);         /* enable INT3 interrupt */

  interrupt_stop ();            /* NB: application level "interrupt" */

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

ISR (INT3_vect)
{
  /*
   * Is it rising or falling edge?
   * falling => enable channel
   * rising => disable channel
   */
  if (PIND & (1 << SHIFTER_NOTIFY))
    channel_disable ();
  else
    channel_enable ();
}
