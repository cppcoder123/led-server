/*
 *
 */
#ifndef FLUSH_H
#define FLUSH_H

#include "data-type.h"

void flush_init ();

uint8_t flush_push_mono (data_t symbol);

enum {
  FLUSH_SHIFT,
  FLUSH_CLEAR,
  FLUSH_DISABLED
};
void flush_enable (uint8_t /*SHIFT/CLEAR*/);

void flush_try ();

#endif
