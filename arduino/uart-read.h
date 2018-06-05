/*
 *
 */
#ifndef UART_READ_H
#define UART_READ_H

#include <stdint.h>

#include "device-id.h"

#include "buffer.h"

/* #define UART_READ_EXTRA_SIZE 50 */
/* #define UART_READ_BUFFER_MAX_SIZE (ID_MAX_MATRIX_SIZE + UART_READ_EXTRA_SIZE) */

/* struct uart_read_buffer */
/* { */
/*   uint8_t data[UART_READ_BUFFER_MAX_SIZE]; */
/*   uint16_t size; */
/*   uint16_t current; */
/* }; */

void uart_read_init ();

volatile struct buffer_t* uart_read_get_buffer ();
  
#endif
