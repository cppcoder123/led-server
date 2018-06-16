/*
 *
 *
 *
 */
#ifndef UART_WRITE_H
#define UART_WRITE_H

#include <stdint.h>

#include "queue.h"

void uart_write_init ();

volatile struct queue_t* uart_write_get_queue ();

void uart_write_kick ();


#endif
