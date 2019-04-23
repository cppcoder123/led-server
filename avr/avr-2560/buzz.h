/*
 *  Noise source
 */
#ifndef BUZZ_H
#define BUZZ_H

#include <stdint.h>

void buzz_init ();

uint8_t buzz_add_data (uint8_t pitch,
                       uint8_t sound_duration,
                       uint8_t silence_duration);

void buzz_try ();

#endif
