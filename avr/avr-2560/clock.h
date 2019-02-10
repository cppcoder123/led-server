/*
 *
 *
 */
#ifndef CLOCK_H
#define CLOCK_H

#include "data-type.h"

/*stops handling of r-pi stream, clears the buffer*/
void clock_sync (data_t hour, data_t min, data_t sec);

#endif
