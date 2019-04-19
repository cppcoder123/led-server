/*
 *
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#include "data-type.h"

/* init internals & start matrix*/
void display_init ();

/* init led matrix */
void display_start ();
void display_stop ();

/*transfer data to led matrix*/
void display_mono_start ();
/*call 'hw_mono' 32 times, for each column*/
void display_mono (data_t data);
void display_mono_stop ();

/*tweak brightness*/
void display_brightness (data_t value/*0-15*/);

#endif
