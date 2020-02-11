/*
 *
 */
#ifndef FLUSH_H
#define FLUSH_H

#include <stdint.h>

void flush_init ();

/* enable & disable flushing globally */
void flush_enable ();
void flush_disable ();

uint8_t flush_push (uint8_t symbol);
uint8_t flush_push_array (uint8_t *arr, uint8_t arr_size);

void flush_enable_shift ();
void flush_enable_stable ();

void flush_try ();

uint8_t flush_buffer_space ();
uint8_t flush_buffer_size ();
void flush_buffer_clear ();

#endif
