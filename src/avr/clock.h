
/*
 *
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

#define CLOCK_HOUR_MAX 23
/* fixme: it seems clock is slower than needed */
#define CLOCK_MINUTE_MAX 58
#define CLOCK_SECOND_MAX 59

void clock_init ();

void clock_set(uint8_t hour, uint8_t min);

void clock_get(uint8_t *hour, uint8_t *min);

void clock_advance_second();

uint8_t clock_alarm_engage(uint8_t hour, uint8_t minute);

void clock_alarm_disengage();

void clock_alarm_get (uint8_t *engaged, uint8_t *hour, uint8_t *minute);

/* Render time (and alarm) into buffer array for displaying */
/* Note: Buffer should be at least 64 bytes long */
void clock_render(uint8_t *buffer);

#endif
