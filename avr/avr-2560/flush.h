/*
 *
 */
#ifndef FLUSH_H
#define FLUSH_H

#include "data-type.h"

void flush_init ();

uint8_t flush_push_mono (data_t symbol);

void flush_enable_shift ();
void flush_enable_clear ();

void flush_try ();

#endif
