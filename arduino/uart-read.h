/*
 *
 */
#ifndef UART_READ_H
#define UART_READ_H

#include <stdint.h>

#include "device-id.h"

#include "queue.h"

void uart_read_init ();

volatile struct queue_t* uart_read_get_queue ();
  
#endif
