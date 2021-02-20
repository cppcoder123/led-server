/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "const/constant.h"

#include "at.h"
#include "buffer.h"
#include "encode.h"
#include "spi.h"

#define MISO PORTB3

/*
 * irq pin is connected to pi's gpio-27
 * This is an artificial interrupt to notify pi that
 * avr wants to write smth into the channel
 */
#define IRQ PORTB4

/*
 * Tell pi that avr is ready
 */
#define CONFIRM PORTB5

/*
 * Port we are using
 */
#define SPI_PORT PORTB

/*
 * Direction register
 */
#define SPI_DDR DDRB

/*
 * How long should we confirm?
 *    ~ 2 seconds
 */
#define CONFIRM_HOLD_TIME 2

static struct buffer_t read_buf;
static struct buffer_t write_buf;

void spi_interrupt_start ()
{
  /*set irq pin 1*/
  SPI_PORT |= (1 << IRQ);
}

static void interrupt_stop ()
{
  SPI_PORT &= ~(1 << IRQ);
}

static void misconfirm ()
{
  /* set confirmation signal to low*/
  SPI_PORT &= ~(1 << CONFIRM);
}

void spi_init ()
{
  /*
   * Configure spi output signals
   */
  SPI_DDR |= (1 << MISO) | (1 << IRQ) | (1 << CONFIRM);

  /*clear*/
  SPDR = 0;

  buffer_init (&read_buf);
  buffer_init (&write_buf);

  /* prepare for confirm */
  misconfirm ();

  /* attach interrupt to shifter enable/disable signal */
  EICRA |= (1 << ISC31);  /* falling edge should generate interrupt */
  EIMSK |= (1 << INT3);   /* enable INT3 interrupt */

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
  buffer_clear (&read_buf);
  buffer_clear (&write_buf);

  SPI_PORT |= (1 << CONFIRM);

  encode_msg_1 (MSG_ID_VERSION, SERIAL_ID_TO_IGNORE, PROTOCOL_VERSION);

  at_schedule (AT_SPI, CONFIRM_HOLD_TIME, &misconfirm);
}
