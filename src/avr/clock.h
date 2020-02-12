
/*
 *
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

void clock_init ();

void clock_set(uint8_t hour, uint8_t min);

void clock_get(uint8_t *hour, uint8_t *min);

void clock_advance_second();

uint8_t clock_alarm_engage(uint8_t hour, uint8_t minute);

void clock_alarm_disengage();

/* Render time (and alarm) into buffer array for displaying */
/* Note: Buffer should be at least 64 bytes long */
void clock_render(uint8_t *buffer);

#endif
