/*
 *
 */
#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

void matrix_init ();

/*returns zero in case of failure*/
uint8_t matrix_update_start (uint8_t type);

uint8_t matrix_update (volatile uint8_t *data, uint8_t data_size);

uint8_t matrix_update_finish (uint8_t type);

void matrix_pixel_delay (uint8_t pixel_delay);

void matrix_phrase_delay (uint8_t phrase_delay);

void matrix_stable_delay (uint8_t delay);

#endif
