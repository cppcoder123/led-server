/*
 *
 */

#include "uart-write.h"

#define BUFFER_MAX_SIZE 50

static uint8_t buffer_data[BUFFER_MAX_SIZE];
static volatile struct buffer_t buffer;

void uart_write_init ()
{
  buffer_init (&buffer, buffer_data, BUFFER_MAX_SIZE);

  /*fixme*/
}

volatile struct buffer_t* uart_write_get_buffer ()
{
  return &buffer;
}

void uart_write_kick ()
{
  /* fixme */
}
