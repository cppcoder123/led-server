/*
 *
 *
 *
 *
 */
#ifndef MATRIX_BUFFER_H
#define MATRIX_BUFFER_H

#include <stdint.h>

#include "queue.h"

void matrix_buffer_init ();

/*type : first, last or middle*/
uint8_t matrix_buffer_fill (uint8_t type, volatile uint8_t *src, uint8_t size);

uint8_t matrix_buffer_drain (volatile uint8_t *type, volatile struct queue_t *sink);

#endif
