/*
 *
 */
#ifndef FLUSH_H
#define FLUSH_H

#include <stdint.h>

#include "buf.h"

#define FLUSH_BRIGHTNESS_MIN 0
#define FLUSH_BRIGHTNESS_MIDDLE 7
#define FLUSH_BRIGHTNESS_MAX 15

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

/*
 * Stable rendering
 */

/* 2 matrices 32x8 points => 64 bytes  */
void flush_stable_display (struct buf_t *buf);

/*
 * Brightness
 */
uint8_t flush_brightness_get ();
void flush_brightness_set (uint8_t brightness);

#endif
