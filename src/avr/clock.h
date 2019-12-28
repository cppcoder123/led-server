/*
 *
 *
 */
#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

void clock_init ();

void clock_try ();

/*sync avr time value*/
void clock_sync (uint8_t hour, uint8_t min);

#endif
