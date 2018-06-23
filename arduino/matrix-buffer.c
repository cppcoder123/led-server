/*
 *
 *
 *
 *
 */
#include <util/atomic.h>

#include "device-id.h"

#include "matrix-buffer.h"
#include "queue.h"

static volatile uint8_t buffer_data[ID_MAX_BUFFER_SIZE];
static volatile struct queue_t buffer;

static uint8_t matrix_type;

void matrix_buffer_init ()
{
  /* debug */
  queue_init (&buffer, buffer_data, ID_MAX_BUFFER_SIZE, 0xAA);

  matrix_type = 0;
}

/* static uint8_t state_updateable (uint8_t type) */
/* { */
/*   /\*first enables update for clean buffer*\/ */
/*   if ((state == 0) */
/*       && ((type & ID_SUB_MATRIX_TYPE_FIRST) == 0)) */
/*     return 0; */

/*   /\*disable update after last*\/ */
/*   if ((state & ID_SUB_MATRIX_TYPE_LAST) != 0) */
/*     return 0; */

/*   return 1; */
/* } */

uint8_t matrix_buffer_fill (uint8_t type, volatile uint8_t *src, uint8_t size)
{
  if (matrix_type != 0)
    /* matrix is not rendered yet */
    return 0;

  uint8_t status = 0;
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    status = queue_refill (&buffer, src, size);
    
    matrix_type = type & ID_SUB_MATRIX_TYPE_MASK;
  }

  return status;
}

/* uint8_t matrix_buffer_update_symbol (uint8_t type, uint8_t symbol) */
/* { */
/*   if (state_updateable (type) == 0) */
/*     return 0; */

/*   if (buffer_size >= ID_MAX_MATRIX_SIZE) */
/*     return 0; */

/*   ATOMIC_BLOCK (ATOMIC_FORCEON) { */
/*     buffer[buffer_size++] = symbol; */
/*     state |= type & ID_SUB_MATRIX_TYPE_MASK; */
/*   } */

/*   return 1; */
/* } */

uint8_t matrix_buffer_drain (volatile uint8_t *type, volatile struct queue_t *sink)
{
  if (matrix_type == 0)
    return 0;

  uint8_t status = 0;
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    *type = matrix_type;
    status = queue_move (&buffer, sink);
    matrix_type = 0;
  }

  return status;
}
