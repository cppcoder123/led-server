/*
 *
 */

#include "flush.h"
#include "font.h"
#include "render.h"

#define DATA_SIZE FLUSH_STABLE_SIZE

void render_empty_column (uint8_t *buffer, uint8_t *position)
{
  if (*position < DATA_SIZE)
    *(buffer + (*position)++) = 0;
}

void render_symbol (uint8_t sym, uint8_t *buffer, uint8_t *position)
{
  if (font_add_symbol (sym, buffer, position, DATA_SIZE) == 0)
    return;

  render_empty_column (buffer, position);
}

static uint8_t is_zero_needed (uint8_t flag, uint8_t order)
{
  return ((flag & order) == order) ? 1 : 0;
}

void render_number (uint8_t num, uint8_t leading_zero,
                    uint8_t *buffer, uint8_t *position)
{
  uint8_t hundred = num / 100;  /* 0, 1, 2 */
  if ((hundred != 0)
      || (is_zero_needed (leading_zero, RENDER_LEADING_HUNDRED) != 0))
    render_symbol (hundred, buffer, position);
  uint8_t rest = num % 100;
  uint8_t ten = rest / 10;
  if ((hundred != 0) || (ten != 0)
      || (is_zero_needed (leading_zero, RENDER_LEADING_TEN) != 0))
    render_symbol (ten, buffer, position);
  render_symbol (rest % 10, buffer, position);
}

void render_word (uint8_t *word, uint8_t word_len,
                  uint8_t *buffer, uint8_t *position)
{
  for (uint8_t i = 0; i < word_len; ++i)
    render_symbol (word[i], buffer, position);
}