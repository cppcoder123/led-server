/*
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart-write.h"

#define BUFFER_MAX_SIZE 50

#define INTERRUPT_FLAG (1 << UDRIE0)

static uint8_t buffer_data[BUFFER_MAX_SIZE];
static volatile struct buffer_t buffer;

void uart_write_init ()
{
  buffer_init (&buffer, buffer_data, BUFFER_MAX_SIZE);
}

volatile struct buffer_t* uart_write_get_buffer ()
{
  return &buffer;
}

void uart_write_kick ()
{
  uint8_t symbol;
  if (buffer_drain_symbol (&buffer, &symbol) == 0)
    /* empty */
    return;

  UDR0 = symbol;

  /*enable transmit interrupt (empty buffer interrupt)*/
  UCSR0B |= INTERRUPT_FLAG;
}

ISR (USART_UDRE_vect)
{
  uint8_t symbol;
  if (buffer_drain_symbol (&buffer, &symbol) == 0) {
    UCSR0B &= ~INTERRUPT_FLAG;   /* disable interrupt  */
    return;
  }

  UDR0 = symbol;
}
