/*
 *
 */

#include "uart-write.h"

volatile struct uart_write_buffer buffer;

void uart_write_init ()
{
  for (uint8_t i = 0; i < UART_WRITE_BUFFER_MAX_SIZE; ++i)
    buffer.data[i] = 0;
  buffer.size = 0;

  /*fixme*/
}

volatile struct uart_write_buffer* uart_write_get_buffer ()
{
  return &buffer;
}

void uart_write_kick ()
{
  /* fixme */
}
