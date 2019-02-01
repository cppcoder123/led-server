/*
 *  Noise source
 */
#ifndef BUZZ_H
#define BUZZ_H

#include "data-type.h"

void buzz_init ();

uint8_t buzz_add_data (data_t pitch,
                       data_t sound_duration,
                       data_t silence_duration);

void buzz_try ();

#endif
