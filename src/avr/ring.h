/*
 *
 */
#ifndef RING_H
#define RING_H

#include <stdint.h>

void ring_init (uint8_t size, volatile uint8_t *data,
                volatile uint8_t *start, volatile uint8_t *finish);

uint8_t ring_size (uint8_t size, uint8_t start, uint8_t finish);

void ring_clear (volatile uint8_t *start, volatile uint8_t *finish);

uint8_t ring_space (uint8_t size, uint8_t start, uint8_t finish);

uint8_t ring_byte_fill (uint8_t size, volatile uint8_t *data,
                        uint8_t start, volatile uint8_t *finish, uint8_t byte);

uint8_t ring_byte_drain (uint8_t size, volatile uint8_t *data,
                         volatile uint8_t *start, uint8_t finish, uint8_t *byte);

uint8_t ring_byte_get (uint8_t size, volatile uint8_t *data,
                       uint8_t start, uint8_t finish,
                       uint8_t index, uint8_t *byte);

uint8_t ring_array_fill (uint8_t size, volatile uint8_t *data,
                         uint8_t start, volatile uint8_t *finish,
                         uint8_t *array, uint8_t array_size);

uint8_t ring_array_drain (uint8_t size, volatile uint8_t *data,
                          volatile uint8_t *start, uint8_t finish,
                          uint8_t *array, uint8_t array_size);

uint8_t ring_is_fillable (uint8_t size,
                          uint8_t start, uint8_t finish, uint8_t fill_size);

uint8_t ring_is_drainable (uint8_t size,
                           uint8_t start, uint8_t finish, uint8_t drain_size);

#endif
