/*
 *
 *
 *
 */
#ifndef UART_WRITE_H
#define UART_WRITE_H

#include <stdint.h>

#define UART_WRITE_BUFFER_MAX_SIZE 50

struct uart_write_buffer
{
  uint8_t data[UART_WRITE_BUFFER_MAX_SIZE];
  uint8_t size;
};

void uart_write_kick ();

#endif
