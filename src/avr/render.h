/*
 *
 */
#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

#include "buf.h"
#include "flush.h"

/* struct render_t */
/* { */
/*   uint8_t data[FLUSH_STABLE_SIZE]; */
/*   uint8_t position; */
/* }; */

/* void render_buffer_init (struct render_t *buf); */

/* uint8_t render_buffer_fill (struct render_t *buf, uint8_t byte); */


uint8_t render_symbol (volatile struct buf_t *buf, uint8_t sym);

enum {
  RENDER_LEADING_DISABLE = 0,
  RENDER_LEADING_TEN = (1 << 0), /* show ten */
  RENDER_LEADING_HUNDRED = ((1 << 1) | RENDER_LEADING_TEN), /* show hundred */
};

uint8_t render_number (volatile struct buf_t *buf, uint8_t num, uint8_t leading_zero);

uint8_t render_word (volatile struct buf_t *buf, uint8_t *word, uint8_t word_len);

/* fill tail with zeros */
void render_tail (volatile struct buf_t *buf);

#endif
