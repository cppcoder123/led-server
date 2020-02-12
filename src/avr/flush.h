/*
 *
 */
#ifndef FLUSH_H
#define FLUSH_H

#include <stdint.h>

void flush_init ();

/*
 * Shift rendering
 */
uint8_t flush_shift_data (uint8_t *arr, uint8_t arr_size);

/* display and shift */
void flush_shift_display ();

void flush_shift_drain_start ();
void flush_shift_drain_stop ();

uint8_t flush_shift_buffer_space ();
uint8_t flush_shift_buffer_size ();
void flush_shift_buffer_clear ();

/*
 * Stable rendering
 */

/* ! array size is 64  */
void flush_stable_data (uint8_t *arr);

void flush_stable_display ();

#endif
