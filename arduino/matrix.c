/*
 *
 *
 *
 *
 */
#include <util/atomic.h>

#include "matrix.h"

void matrix_init (volatile struct matrix_t *matrix,
                  volatile struct queue_t *queue,
                  volatile uint8_t *buffer,
                  uint8_t buffer_size,
                  uint8_t pattern)
{
  queue_init (queue, buffer, buffer_size, pattern);
  matrix->queue = queue;
  matrix->type = 0;
}

uint8_t matrix_size (volatile struct matrix_t *matrix)
{
  return queue_size (matrix->queue);
}

uint8_t matrix_fill (volatile struct matrix_t *matrix,
                     uint8_t type, volatile uint8_t *data, uint8_t size)
{
  if (queue_size (matrix->queue) != 0)
    return 0;

  uint8_t status = 0;
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    matrix->type = type;
    status = queue_refill (matrix->queue, data, size);
  }

  return status;
}

uint8_t matrix_move (volatile struct matrix_t *from,
                     volatile struct matrix_t *to)
{
  uint8_t status = 0;
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    to->type = from->type;
    status = queue_move (from->queue, to->queue);
  }

  return status;
}

void matrix_clear (volatile struct matrix_t *matrix)
{
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    matrix->type = 0;
    queue_clear (matrix->queue);
  }
}

uint8_t matrix_get (volatile struct matrix_t *matrix,
                    uint8_t index, volatile uint8_t **data)
{
  return queue_get (matrix->queue, index, data);
}
