/*
 *
 */
#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

#include "buf.h"
#include "flush.h"

enum {
  RENDER_LEADING_DISABLE = 0,
  RENDER_LEADING_TEN = (1 << 0), /* show 'tens' unconditionally */
  RENDER_LEADING_HUNDRED = ((1 << 1) | RENDER_LEADING_TEN), /* show hundred */
};

uint8_t render_symbol (struct buf_t *buf, uint8_t sym);

uint8_t render_number (struct buf_t *buf,
                       uint8_t num, uint8_t leading_zero/*see enum*/);

uint8_t render_word (struct buf_t *buf,
                     uint8_t *word, uint8_t word_len);

/* fill tail with zeros */
void render_tail (struct buf_t *buf);

#endif
