/*
 *
 */
#ifndef FLUSH_HW_H
#define FLUSH_HW_H

#include <stdint.h>

#include "data-type.h"

/* init internals & start matrix*/
void flush_hw_init ();

/* init led matrix */
void flush_hw_start ();
void flush_hw_stop ();

/*transfer data to led matrix*/
void flush_hw_mono_start ();
/*call 'hw_mono' 32 times, for each column*/
void flush_hw_mono (data_t data);
void flush_hw_mono_stop ();

/*tweak brightness*/
void flush_hw_brightness (data_t value/*0-15*/);

#endif
