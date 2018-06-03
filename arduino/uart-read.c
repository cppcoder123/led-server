/*
 *
 */

#include "uart-read.h"

volatile struct uart_read_buffer buffer;

void uart_read_init ()
{
  for (uint16_t i = 0; i < UART_READ_BUFFER_MAX_SIZE; ++i)
    buffer.data[i] = 0;
  buffer.size = buffer.current = 0;
  /* fixme   */
}

volatile struct uart_read_buffer* uart_read_get_buffer ()
{
  return &buffer;
}
