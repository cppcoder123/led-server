/*
 *
 */

#include "uart-read.h"

#define BUFFER_SIZE 0xFF

static uint8_t buffer_data[BUFFER_SIZE];
static volatile struct buffer_t buffer;

void uart_read_init ()
{
  buffer_init (&buffer, buffer_data, BUFFER_SIZE);
  
  /* fixme   */
}

volatile struct buffer_t* uart_read_get_buffer ()
{
  return &buffer;
}
