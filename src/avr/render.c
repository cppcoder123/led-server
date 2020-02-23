/*
 *
 */

#include "font.h"
#include "render.h"

#define DATA_SIZE FLUSH_STABLE_SIZE

void render_buffer_init (struct render_t *buf)
{
  buf->position = 0;

  for (uint8_t i = 0; i < DATA_SIZE; ++i)
    buf->data[i] = 0;
}

uint8_t render_buffer_fill (struct render_t *buf, uint8_t byte)
{
  if (buf->position >= DATA_SIZE)
    return 0;

  buf->data[buf->position++] = byte;
  return 1;
}

uint8_t render_symbol (struct render_t *buf, uint8_t symbol)
{
  return ((font_add_symbol (symbol, buf) != 0)
          && (render_buffer_fill (buf, 0) != 0)) ? 1 : 0;
}

static uint8_t is_zero_needed (uint8_t flag, uint8_t order)
{
  return ((flag & order) == order) ? 1 : 0;
}

uint8_t render_number (struct render_t *buf, uint8_t num, uint8_t leading_zero)
{
  uint8_t hundred = num / 100;  /* 0, 1, 2 */
  if (((hundred != 0)
       || (is_zero_needed (leading_zero, RENDER_LEADING_HUNDRED) != 0))
      && (render_symbol (buf, hundred) == 0))
    return 0;

  uint8_t rest = num % 100;
  uint8_t ten = rest / 10;
  if (((hundred != 0)
       || (ten != 0)
       || (is_zero_needed (leading_zero, RENDER_LEADING_TEN) != 0))
      && (render_symbol (buf, ten) == 0))
    return 0;

  return render_symbol (buf, rest % 10);
}

uint8_t render_word (struct render_t *buf, uint8_t *word, uint8_t word_len)
{
  for (uint8_t i = 0; i < word_len; ++i)
    if (render_symbol (buf, word[i]) == 0)
      return 0;

  return 1;
}

void render_empty_tail (struct render_t *buf)
{
  while ((buf->position) < DATA_SIZE)
    render_buffer_fill (buf, 0);
}
