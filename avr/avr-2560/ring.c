/*
 *
 */

#include <util/atomic.h>

#include "mcu/constant.h"

#include "ring.h"

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

static uint8_t get_capacity (volatile data_t *buf)
{
  return buf[SIZE] - SHIFT;
}

static uint8_t get_size (volatile data_t *buf)
{
  if (buf[FULL] != 0)
    return get_capacity (buf);

  if (buf[FINISH] >= buf[START])
    return buf[FINISH] - buf[START];

  // finish is less than start
  return get_capacity (buf) - (buf[START] - buf[FINISH]);
}

static uint8_t is_fillable (volatile data_t *buf, uint8_t fill_size)
{
  if (buf[FULL] != 0)
    return 0;

  return (get_capacity (buf) - get_size (buf) >= fill_size) ? 1 : 0;
}

static uint8_t is_drainable (volatile data_t *buf, uint8_t drain_size)
{
  return (get_size (buf) >= drain_size) ? 1 : 0;
}

static void fill_symbol (volatile data_t *buf, uint8_t symbol)
{
  buf[buf[FINISH]] = symbol;
  ++buf[FINISH];
  if (buf[FINISH] == buf[SIZE])
    // wrap
    buf[FINISH] = SHIFT;
  if (buf[FINISH] == buf[START])
    buf[FULL] = 1;
}

static uint8_t drain_symbol (volatile data_t *buf)
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

void ring_init (volatile data_t *buf, uint8_t max_size)
{
  buf[SIZE] = max_size;
  buf[START] = SHIFT;
  buf[FINISH] = SHIFT;
  buf[FULL] = 0;

  /*#ifdef RING_DEBUG*/
  for (uint8_t i = SHIFT; i < max_size; ++i)
    buf[i] = RING_INIT_PATTERN;
  /*#endif*/
}

uint8_t ring_size (volatile data_t *buf)
{
  uint8_t tmp;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    tmp = get_size (buf);
  }

  return tmp;
}

void ring_clear (volatile data_t *buf)
{
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    ring_init (buf, buf[SIZE]);
  }
}

uint8_t ring_space (volatile data_t *buf)
{
  // capacity - size
  uint8_t space;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    space = get_capacity (buf) - get_size (buf);
  }

  return space;
}

uint8_t ring_symbol_fill (volatile data_t *buf, data_t symbol)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if ((status = is_fillable (buf, 1)) != 0)
      fill_symbol (buf, symbol);
  }

  return status;
}

uint8_t ring_symbol_drain (volatile data_t *buf, data_t *symbol)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if ((status = is_drainable (buf, 1)) != 0)
      *symbol = drain_symbol (buf);
  }

  return status;
}

uint8_t ring_symbol_get (volatile data_t *buf, uint8_t index, data_t *symbol)
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

uint8_t ring_array_fill (volatile data_t *ring,
                         data_t *arr, uint8_t fill_size)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if ((status = is_fillable (ring, fill_size)) != 0)
      for (uint8_t i = 0; i < fill_size; ++i)
        fill_symbol (ring, arr[i]);
  }

  return status;
}

uint8_t ring_array_drain (volatile data_t *ring,
                          data_t *arr, uint8_t drain_size)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if ((status = is_drainable (ring, drain_size)) != 0)
      for (uint8_t i = 0; i < drain_size; ++i)
        arr[i] = drain_symbol (ring);
  }

  return status;
}

uint8_t ring_is_fillable (volatile data_t *buf, uint8_t fill_size)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    status = is_fillable (buf, fill_size);
  }

  return status;
}

uint8_t ring_is_drainable (volatile data_t *buf, uint8_t drain_size)
{
  uint8_t status;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    status = is_drainable (buf, drain_size);
  }

  return status;
}


#if 0
uint8_t ring_get (volatile struct ring_t *buf, uint8_t index,
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
uint8_t ring_drain (volatile struct ring_t *buf, uint8_t drain_size)
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

uint8_t ring_refill (volatile struct ring_t *buf,
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

uint8_t ring_move (volatile struct ring_t *from,
                    volatile struct ring_t *to)
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

void ring_clear (volatile struct ring_t *buf)
{
  buf->size = 0;
}

uint8_t ring_size (volatile struct ring_t *buf)
{
  return buf->size;
}
#endif
