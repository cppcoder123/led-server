/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "unix/constant.h"

#include "buffer.h"
#include "debug.h"
#include "invoke.h"
#include "spi.h"

#define SPI_MISO PORTB3
/* irq pin is connected to pi's gpio-27 */
#define SPI_IRQ PORTB6

#define FLAG_WRITE_INTERRUPT (1 << 0)
#define FLAG_HEARTBEAT_ENGAGED (1 << 1)
#define FLAG_HEARTBEAT_CHECK (1 << 2)

#define TWICE_PER_SECOND 100

static volatile struct buffer_t read_buf;
static volatile struct buffer_t write_buf;

static volatile uint8_t flag;

static spi_disconnect_callback callback;

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

  buffer_init (&read_buf);
  buffer_init (&write_buf);

  flag = FLAG_WRITE_INTERRUPT;
  write_interrupt_stop ();

  /*enable spi, and enable related interrupt*/
  SPCR = (1 << SPIE) | (1 << SPE);

  callback = 0;
}

uint8_t spi_read_symbol (uint8_t *symbol)
{
  /* return queue_symbol_drain (read_buf, symbol); */
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
  write_interrupt_start ();
  return status;
}

void heartbeat_check ()
{
  if ((flag & FLAG_HEARTBEAT_CHECK) != 0) {
    if (callback)
      callback ();
    buffer_clear (&write_buf);
    invoke_disable (INVOKE_ID_SPI);
    return;
  }

  flag |= FLAG_HEARTBEAT_CHECK;
  write_interrupt_start ();
}

void heartbeat_confirm ()
{
  flag &= ~FLAG_HEARTBEAT_CHECK;

  if ((flag & FLAG_HEARTBEAT_ENGAGED) == 0) {
    flag |= FLAG_HEARTBEAT_ENGAGED;
    invoke_enable (INVOKE_ID_SPI, TWICE_PER_SECOND, heartbeat_check);
  }
}

void spi_note_disconnect (spi_disconnect_callback cb)
{
  callback = cb;
}

ISR (SPI_STC_vect)
{
  heartbeat_confirm ();

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
    write_interrupt_stop ();
  }
}
