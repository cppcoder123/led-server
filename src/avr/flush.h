/*
 *
 */
#ifndef FLUSH_H
#define FLUSH_H

#include <stdint.h>

void flush_init ();

uint8_t flush_push_array (uint8_t *arr, uint8_t arr_size);

void flush_shift_enable ();
void flush_shift_disable();

void flush_stable_enable ();
void flush_stable_disable ();
void flush_stable_display ();

uint8_t flush_buffer_space ();
uint8_t flush_buffer_size ();
void flush_buffer_clear ();

#endif
