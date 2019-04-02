/*
 *
 */
#ifndef BITBANG_H
#define BITBANG_H

#include <stdint.h>

#include "data-type.h"

/* init internals & start matrix*/
void bitbang_init ();

/* init led matrix */
void bitbang_start ();
void bitbang_stop ();

/*transfer data to led matrix*/
void bitbang_mono_start ();
/*call 'hw_mono' 32 times, for each column*/
void bitbang_mono (data_t data);
void bitbang_mono_stop ();

/*tweak brightness*/
void bitbang_brightness (data_t value/*0-15*/);

#endif
