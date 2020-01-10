/*
 *  Noise source
 */
#ifndef BUZZ_H
#define BUZZ_H

#include <stdint.h>

void buzz_init ();

void buzz_start();
void buzz_stop();

/* clear melody buffer */
void buzz_clear();
/* Note 1: volume should be less or equal to pitch */
/* Note 2: Duration: 1 second equals approx to 3906 */
uint8_t buzz_add_sound(uint8_t pitch, uint8_t volume, uint16_t duration);
uint8_t buzz_add_pause(uint16_t duration);
/* repeat melody num times */
void buzz_repeat(uint8_t num);

#endif
