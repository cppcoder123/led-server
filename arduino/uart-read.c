/*
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#include "uart-read.h"

#define QUEUE_SIZE 0xFF

static uint8_t queue_data[QUEUE_SIZE];
static volatile struct queue_t queue;

void uart_read_init ()
{
  queue_init (&queue, queue_data, QUEUE_SIZE, 44);
}

volatile struct queue_t* uart_read_get_queue ()
{
  return &queue;
}

ISR (USART_RX_vect)
{
  /* what we can do with error code  here? */
   queue_fill_symbol (&queue, UDR0);
}
