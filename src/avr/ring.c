/*
 *
 */

#include <util/atomic.h>

#include "unix/constant.h"

#include "ring.h"

void ring_init (uint8_t size, volatile uint8_t *data,
                volatile uint8_t *start, volatile uint8_t *finish)
{
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    for (uint8_t i = 0; i < size; ++i)
      *(data + i) = BUFFER_INIT_PATTERN;
    *start = *finish = 0;
  }
}

uint8_t ring_size (uint8_t size, uint8_t start, uint8_t finish)
{
  uint8_t result = 0;

  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    result = (finish >= start) ? finish - start : size - start + finish;
  }

  return result;
}

void ring_clear (volatile uint8_t *start, volatile uint8_t *finish)
{
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    *start = *finish = 0;
  }
}

uint8_t ring_space (uint8_t size, uint8_t start, uint8_t finish)
{
  uint8_t result = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    result = size - ring_size (size, start, finish);
  }

  return result;
}

uint8_t ring_byte_fill (uint8_t size, volatile uint8_t *data,
                        uint8_t start, volatile uint8_t *finish, uint8_t byte)
{
  uint8_t result = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if (ring_is_fillable (size, start, *finish, 1) != 0) {
      if (*finish < size - 1) {
        *(data + *finish) = byte;
        ++(*finish);
      } else {
        /* fill data at 0 position */
        *data = byte;
        *finish = 1;
      }

      result = 1;
    }
  }

  return result;
}

uint8_t ring_byte_drain (uint8_t size, volatile uint8_t *data,
                         volatile uint8_t *start, uint8_t finish, uint8_t *byte)
{
  uint8_t result = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if (ring_is_drainable (size, *start, finish, 1) != 0) {

      *byte = *(data + *start);

      if (*start < size - 1)
        ++(*start);
      else
        *start = 0;

      result = 1;
    }
  }

  return result;
}

uint8_t ring_byte_get (uint8_t size, volatile uint8_t *data,
                       uint8_t start, uint8_t finish,
                       uint8_t index, uint8_t *byte)
{
  uint8_t result = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if (ring_is_drainable (size, start, finish, index) != 0) {

      uint16_t long_index = (uint16_t)start + (uint16_t)index;
  
      if (long_index < size)
        *byte = *(data + start + index);
      else {
        uint8_t new_index = (uint8_t) (long_index - size);
        *byte = *(data + new_index);
      }

      result = 1;
    }
  }

  return result;
}

uint8_t ring_array_fill (uint8_t size, volatile uint8_t *data,
                         uint8_t start, volatile uint8_t *finish,
                         uint8_t *array, uint8_t array_size)
{
  uint8_t result = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if (ring_is_fillable (size, start, *finish, array_size) != 0) {
      for (uint8_t i = 0; i < array_size; ++i)
        ring_byte_fill (size, data, start, finish, *(array + i));

      result = 1;
    }
  }

  return result;
}

uint8_t ring_array_drain (uint8_t size, volatile uint8_t *data,
                          volatile uint8_t *start, uint8_t finish,
                          uint8_t *array, uint8_t array_size)
{
  uint8_t result = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if (ring_is_drainable (size, *start, finish, array_size) != 0) {
      for (uint8_t i = 0; i < array_size; ++i) {
        uint8_t byte = 0;
        if (ring_byte_drain (size, data, start, finish, &byte) != 0)
          *(array + i) = byte;
      }

      result = 1;
    }
  }

  return result;
}

uint8_t ring_is_fillable (uint8_t size,
                          uint8_t start, uint8_t finish, uint8_t fill_size)
{
  uint8_t result = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    uint8_t free = size - ring_size (size, start, finish);
    result = (fill_size <= free) ? 1 : 0;
  }
  return result;
}

uint8_t ring_is_drainable (uint8_t size,
                           uint8_t start, uint8_t finish, uint8_t drain_size)
{
  uint8_t result = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    result = (drain_size < ring_size (size, start, finish)) ? 1 : 0;
  }
  return result;
}
