/*
 *
 */
#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

void render_empty_column (uint8_t *buffer, uint8_t *position);

void render_symbol (uint8_t sym, uint8_t *buffer, uint8_t *position);

enum {
  RENDER_LEADING_DISABLE = 0,
  RENDER_LEADING_TEN = (1 << 0), /* show ten */
  RENDER_LEADING_HUNDRED = ((1 << 1) | RENDER_LEADING_TEN), /* show hundred */
};

void render_number (uint8_t num, uint8_t leading_zero,
                    uint8_t *buffer, uint8_t *position);

void render_word (uint8_t *word, uint8_t word_len,
                  uint8_t *buffer, uint8_t *position);

#endif