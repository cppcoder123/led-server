/*
 *
 *
 *
 *
 */
#include <util/atomic.h>

#include "device-id.h"

#include "matrix-buffer.h"

static volatile uint8_t buffer[ID_MAX_MATRIX_SIZE];
static uint16_t buffer_size;
static uint8_t state;

void matrix_buffer_init ()
{
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    /* debug */
    for (uint16_t i = 0; i < ID_MAX_MATRIX_SIZE; ++i)
      buffer[i] = 0xFF;

    buffer_size = 0;

    state = 0;
  }
}

static uint8_t state_updateable (uint8_t type)
{
  /*first enables update for clean buffer*/
  if ((state == 0)
      && ((type & ID_SUB_MATRIX_TYPE_FIRST) == 0))
    return 0;

  /*disable update after last*/
  if ((state & ID_SUB_MATRIX_TYPE_LAST) != 0)
    return 0;

  return 1;
}

uint8_t matrix_buffer_update (uint8_t type, volatile uint8_t *src, uint8_t size)
{
  if (state_updateable (type) == 0)
    return 0;

  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    for (uint8_t i = 0; i < size; ++i)
      buffer[buffer_size++] = *(src + i);

    state |= type & ID_SUB_MATRIX_TYPE_MASK;
  }

  return 1;
}

uint8_t matrix_buffer_update_symbol (uint8_t type, uint8_t symbol)
{
  if (state_updateable (type) == 0)
    return 0;

  if (buffer_size >= ID_MAX_MATRIX_SIZE)
    return 0;

  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    buffer[buffer_size++] = symbol;
    state |= type & ID_SUB_MATRIX_TYPE_MASK;
  }

  return 1;
}

uint8_t matrix_buffer_drain (matrix_buffer_array_t sink,
                             volatile uint16_t *size)
{
  if ((state & ID_SUB_MATRIX_TYPE_LAST) == 0)
    /*either empty or not completely updated*/
    return 0;

  for (uint16_t i = 0; i < buffer_size; ++i)
    (*sink)[i] = buffer[i];

  *size = buffer_size;

  matrix_buffer_init ();

  return 1;
}
