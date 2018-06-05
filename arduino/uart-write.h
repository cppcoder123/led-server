/*
 *
 *
 *
 */
#ifndef UART_WRITE_H
#define UART_WRITE_H

#include <stdint.h>

#include "buffer.h"

void uart_write_init ();

volatile struct buffer_t* uart_write_get_buffer ();

void uart_write_kick ();


#endif
