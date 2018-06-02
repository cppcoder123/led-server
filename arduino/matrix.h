/*
 *
 */
#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

/*returns zero in case of failure*/
uint8_t matrix_write_prepare (uint16_t size);

void matrix_write (volatile uint8_t *data);

void matrix_shift_delay (uint8_t pixel_delay, uint8_t phrase_delay);

void matrix_stable_delay (uint8_t delay);

#endif
