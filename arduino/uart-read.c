/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#include "uart-read.h"

#define BUFFER_SIZE 0xFF

static uint8_t buffer_data[BUFFER_SIZE];
static volatile struct buffer_t buffer;

void uart_read_init ()
{
  buffer_init (&buffer, buffer_data, BUFFER_SIZE, 44);
}

volatile struct buffer_t* uart_read_get_buffer ()
{
  return &buffer;
}

ISR (USART_RX_vect)
{
  /* what we can do with error code  here? */
   buffer_fill_symbol (&buffer, UDR0);
}
