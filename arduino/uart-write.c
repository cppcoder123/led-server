/*
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart-write.h"

#define QUEUE_MAX_SIZE 50

static uint8_t queue_data[QUEUE_MAX_SIZE];
static volatile struct queue_t queue;

void uart_write_init ()
{
  queue_init (&queue, queue_data, QUEUE_MAX_SIZE, 222);
}

volatile struct queue_t* uart_write_get_queue ()
{
  return &queue;
}

void uart_write_kick ()
{
  uint8_t symbol;
  if (queue_drain_symbol (&queue, &symbol) == 0)
    /* empty */
    return;

  /* Wait for empty transmit buffer */
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  
  UDR0 = symbol;
}

ISR (USART_UDRE_vect)
{
  uint8_t symbol;
  if (queue_drain_symbol (&queue, &symbol) == 0) {
    /* UCSR0B &= ~INTERRUPT_FLAG;   /\* disable interrupt  *\/ */
    return;
  }

  UDR0 = symbol;
}
