/*
 *
 */
#ifndef FLUSH_H
#define FLUSH_H

#include "data-type.h"

void flush_init ();

/* enable & disable flushing globally */
void flush_enable ();
void flush_disable ();

uint8_t flush_push_mono (data_t symbol);

void flush_enable_shift ();
void flush_enable_clear ();

void flush_try ();

#endif
