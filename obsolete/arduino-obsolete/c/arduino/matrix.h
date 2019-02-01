/*
 *
 *
 *
 *
 */
#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

#include "queue.h"

struct matrix_t
{
  volatile struct queue_t *queue;
  uint8_t type;
};

void matrix_init (volatile struct matrix_t *matrix,
                  volatile struct queue_t *queue,
                  volatile uint8_t *buffer,
                  uint8_t buffer_size,
                  uint8_t pattern);


uint8_t matrix_size (volatile struct matrix_t *matrix);

/* if 'matrix' is not empty, it just returns 0 */
/* type : first, last or middle*/
uint8_t matrix_fill (volatile struct matrix_t *matrix,
                     uint8_t type, volatile uint8_t *src, uint8_t size);

uint8_t matrix_move (volatile struct matrix_t *from,
                     volatile struct matrix_t *to);

void matrix_clear (volatile struct matrix_t *matrix);

uint8_t matrix_get (volatile struct matrix_t *matrix,
                    uint8_t index, volatile uint8_t **data);

#endif
