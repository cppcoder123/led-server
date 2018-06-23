/*
 *
 */

#include <util/atomic.h>

#include "queue.h"


void queue_init (volatile struct queue_t *buf,
                 volatile uint8_t *data,
                 uint8_t max_size, uint8_t pattern)
{
  buf->data = data;
  buf->size = 0;
  buf->max_size = max_size;

  /* debug */
  for (uint8_t i = 0; i < buf->max_size; ++i)
    buf->data[i] = pattern;
}

uint8_t queue_is_fillable (volatile struct queue_t *buf, uint8_t fill_size)
{
  return (buf->size + fill_size <= buf->max_size) ? 1 : 0;
}

uint8_t queue_is_drainable (volatile struct queue_t *buf, uint8_t drain_size)
{
  return (buf->size >= drain_size) ? 1 : 0;
}

uint8_t queue_get (volatile struct queue_t *buf, uint8_t index,
                   volatile uint8_t **data)
{
  if (index >= buf->size)
    // outside of data
    return 0;

  *data = buf->data + index;

  return 1;
}

uint8_t queue_fill_symbol (volatile struct queue_t *buf, uint8_t symbol)
{
  if (buf->size >= buf->max_size)
    // Do we have space ?
    return 0;

  ATOMIC_BLOCK (ATOMIC_FORCEON)
  {
    buf->data[buf->size] = symbol;
    ++(buf->size);
  }

  return 1;
}

uint8_t queue_drain_symbol (volatile struct queue_t *buf, uint8_t *symbol)
{
  if (buf->size == 0)
    return 0;

  ATOMIC_BLOCK (ATOMIC_FORCEON)
  {
    *symbol = buf->data[0];
    queue_drain (buf, 1);
  }

  return 1;
}

uint8_t queue_drain (volatile struct queue_t *buf, uint8_t drain_size)
{
  if (drain_size > buf->size)
    return 0;

  ATOMIC_BLOCK (ATOMIC_FORCEON)
  {
    uint8_t delta = buf->size - drain_size;

    for (uint8_t i = 0; i < delta; ++i)
      buf->data[i] = buf->data[i + drain_size];

    buf->size = delta;
  }

  return 1;
}

uint8_t queue_refill (volatile struct queue_t *buf,
                      volatile uint8_t *src, uint8_t src_size)
{
  if (buf->max_size < src_size)
    return 0;

  ATOMIC_BLOCK (ATOMIC_FORCEON)
  {
    for (uint8_t i = 0; i < src_size; ++i)
      buf->data[i] = src[i];

    buf->size = src_size;
  }

  return 1;
}

uint8_t queue_move (volatile struct queue_t *from,
                    volatile struct queue_t *to)
{
  if (to->max_size < from->size)
    return 0;

  ATOMIC_BLOCK (ATOMIC_FORCEON)
  {
    for (uint8_t i = 0; i < from->size; ++i)
      to->data[i] = from->data[i];
    to->size = from->size;
    from->size = 0;
  }
  
  return 1;
}

void queue_clear (volatile struct queue_t *buf)
{
  buf->size = 0;
}

uint8_t queue_size (volatile struct queue_t *buf)
{
  return buf->size;
}
