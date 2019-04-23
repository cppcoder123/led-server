/*
 *
 *
 */
#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

/*stops handling of r-pi stream, clears the buffer*/
void clock_sync (uint8_t hour, uint8_t min, uint8_t sec);

#endif
