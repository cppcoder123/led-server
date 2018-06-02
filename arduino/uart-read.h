/*
 *
 */
#ifndef UART_READ_H
#define UART_READ_H

#include <stdint.h>

#include "device-id.h"

#define UART_READ_EXTRA_SIZE 50

struct uart_read_buffer
{
  uint8_t data[ID_MAX_MATRIX_SIZE + UART_READ_EXTRA_SIZE];
  uint16_t size;
  uint16_t current;
};
  
#endif
