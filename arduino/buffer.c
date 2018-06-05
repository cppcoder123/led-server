/*
 *
 */

#include "buffer.h"


void buffer_init (volatile struct buffer_t *buf,
                  uint8_t *data, uint8_t max_size)
{
  buf->data = data;
  buf->size = 0;
  buf->max_size = max_size;
}

uint8_t buffer_is_fillable (volatile struct buffer_t *buf, uint8_t fill_size)
{
  return (buf->size + fill_size <= buf->max_size) ? 1 : 0;
}

uint8_t buffer_is_drainable (volatile struct buffer_t *buf, uint8_t drain_size)
{
  return (buf->size >= drain_size) ? 1 : 0;
}

uint8_t buffer_get (volatile struct buffer_t *buf, uint8_t index, uint8_t **data)
{
  if (index >= buf->size)
    // outside of data
    return 0;

  *data = buf->data + index;

  return 1;
}

uint8_t buffer_fill_symbol (volatile struct buffer_t *buf, uint8_t symbol)
{
  if (buf->size + 1 >= buf->max_size)
    // Do we have space ?
    return 0;

  buf->data[buf->size++] = symbol;
  
  return 1;
}

uint8_t buffer_drain_symbol (volatile struct buffer_t *buf, uint8_t *symbol)
{
  if (buf->size == 0)
    return 0;

  *symbol = buf->data[0];

  return buffer_drain (buf, 1);
}

uint8_t buffer_drain (volatile struct buffer_t *buf, uint8_t drain_size)
{
  if (drain_size > buf->size)
    return 0;

  uint8_t delta = buf->size - drain_size;
  
  for (uint8_t i = 0; i < delta; ++i)
    buf->data[i] = buf->data[i + delta];

  buf->size = delta;
  
  return 1;
}
