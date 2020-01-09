
/*
 *
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

void clock_init ();

void clock_set(uint8_t hour, uint8_t min);

void clock_get(uint8_t *hour, uint8_t *min);

/* return not zero if minute value is changed */
uint8_t clock_advance_second();

#endif
