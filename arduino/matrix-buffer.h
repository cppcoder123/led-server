/*
 *
 *
 *
 *
 */
#ifndef MATRIX_BUFFER_H
#define MATRIX_BUFFER_H

#include <stdint.h>

#include "device-id.h"


void matrix_buffer_init ();

/*type : first or last or middle*/
uint8_t matrix_buffer_update (uint8_t type, volatile uint8_t *src, uint8_t size);

uint8_t matrix_buffer_update_symbol (uint8_t type, uint8_t symbol);

/* ? */
typedef volatile uint8_t(*matrix_buffer_array_t)[ID_MAX_MATRIX_SIZE];
uint8_t matrix_buffer_drain (matrix_buffer_array_t sink,
                             volatile uint16_t *size);

#endif
