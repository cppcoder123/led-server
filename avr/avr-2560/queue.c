/*
 *
 */

#include <util/atomic.h>

#include "mcu/constant.h"

#include "queue.h"

/*
 * Treat array as ring buffer:
 *
 * [0] - is size,
 * [1] - start,
 * [2] - finish
 * [3] - full flag
 */

#define SIZE 0
#define START 1
#define FINISH 2
#define FULL 3
#define SHIFT 4

static uint8_t get_capacity (volatile uint8_t *buf)
{
  return buf[SIZE] - SHIFT;
}

static uint8_t get_size (volatile uint8_t *buf)
{
  if (buf[FULL] != 0)
    return get_capacity (buf);

  if (buf[FINISH] >= buf[START])
    return buf[FINISH] - buf[START];

  // finish is less than start
  return get_capacity (buf) - (buf[START] - buf[FINISH]);
}

static uint8_t is_fillable (volatile uint8_t *buf, uint8_t fill_size)
{
  if (buf[FULL] != 0)
    return 0;

  return (get_capacity (buf) - get_size (buf) >= fill_size) ? 1 : 0;
}

static uint8_t is_drainable (volatile uint8_t *buf, uint8_t drain_size)
{
  return (get_size (buf) >= drain_size) ? 1 : 0;
}

static void fill_symbol (volatile uint8_t *buf, uint8_t symbol)
{
  buf[buf[FINISH]] = symbol;
  ++buf[FINISH];
  if (buf[FINISH] == buf[SIZE])
    // wrap
    buf[FINISH] = SHIFT;
  if (buf[FINISH] == buf[START])
    buf[FULL] = 1;
}

static uint8_t drain_symbol (volatile uint8_t *buf)
{
  uint8_t symbol = buf[buf[START]];
  ++buf[START];

  if (buf[START] == buf[SIZE])
    buf[START] = SHIFT;
  if (buf[FULL] == 1)
    buf[FULL] = 0;

  return symbol;
}

/*------------------------------*/

void queue_init (volatile uint8_t *buf, uint8_t max_size)
{
  buf[SIZE] = max_size;
  buf[START] = SHIFT;
  buf[FINISH] = SHIFT;
  buf[FULL] = 0;

  /*#ifdef QUEUE_DEBUG*/
  for (uint8_t i = SHIFT; i < max_size; ++i)
    buf[i] = QUEUE_INIT_PATTERN;
  /*#endif*/
}

uint8_t queue_size (volatile uint8_t *buf)
{
  uint8_t tmp;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    tmp = get_size (buf);
  }

  return tmp;
}

void queue_clear (volatile uint8_t *buf)
{
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    queue_init (buf, buf[SIZE]);
  }
}

uint8_t queue_space (volatile uint8_t *buf)
{
  // capacity - size
  uint8_t space;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    space = get_capacity (buf) - get_size (buf);
  }

  return space;
}

uint8_t queue_symbol_fill (volatile uint8_t *buf, uint8_t symbol)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if ((status = is_fillable (buf, 1)) != 0)
      fill_symbol (buf, symbol);
  }

  return status;
}

uint8_t queue_symbol_drain (volatile uint8_t *buf, uint8_t *symbol)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if ((status = is_drainable (buf, 1)) != 0)
      *symbol = drain_symbol (buf);
  }

  return status;
}

uint8_t queue_symbol_get (volatile uint8_t *buf, uint8_t index, uint8_t *symbol)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if ((status = is_drainable (buf, index + 1) != 0)) {
      uint8_t j = buf[START];
      for (uint8_t i = 0; i < index; ++i)
        if (++j == buf[SIZE])     /* wrap */
          j = SHIFT;
      *symbol = buf[j];
    }
  }

  return status;
}

uint8_t queue_array_fill (volatile uint8_t *queue,
                         uint8_t *arr, uint8_t fill_size)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if ((status = is_fillable (queue, fill_size)) != 0)
      for (uint8_t i = 0; i < fill_size; ++i)
        fill_symbol (queue, arr[i]);
  }

  return status;
}

uint8_t queue_array_drain (volatile uint8_t *queue,
                          uint8_t *arr, uint8_t drain_size)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if ((status = is_drainable (queue, drain_size)) != 0)
      for (uint8_t i = 0; i < drain_size; ++i)
        arr[i] = drain_symbol (queue);
  }

  return status;
}

uint8_t queue_is_fillable (volatile uint8_t *buf, uint8_t fill_size)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    status = is_fillable (buf, fill_size);
  }

  return status;
}

uint8_t queue_is_drainable (volatile uint8_t *buf, uint8_t drain_size)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    status = is_drainable (buf, drain_size);
  }

  return status;
}


#if 0
uint8_t queue_get (volatile struct queue_t *buf, uint8_t index,
                   volatile uint8_t **data)
{
  if (index >= buf->size)
    // outside of data
    return 0;

  *data = buf->data + index;

  return 1;
}
#endif


#if 0
uint8_t queue_drain (volatile struct queue_t *buf, uint8_t drain_size)
{
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if (is_drainable (buf, drain_size) == 0)
      return 0;

    if (drain_size > buf->size)
    return 0;

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

  ATOMIC_BLOCK (ATOMIC_RESTORESTATE)
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

  ATOMIC_BLOCK (ATOMIC_RESTORESTATE)
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
#endif
