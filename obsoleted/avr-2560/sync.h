/*
 *
 *
 */
#ifndef SYNC_H
#define SYNC_H

#include <stdint.h>

void sync_init ();

/*sync avr time value*/
void sync_clock (uint8_t hour, uint8_t min);

#endif
