/*
 *
 */
#ifndef UART_READ_H
#define UART_READ_H

#include <stdint.h>

#include "device-id.h"

#include "buffer.h"

void uart_read_init ();

volatile struct buffer_t* uart_read_get_buffer ();
  
#endif
