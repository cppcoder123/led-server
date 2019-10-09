/*
 *
 */
#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

/* Do not access directly use functions instead */
struct queue_t
{
  volatile uint8_t *data;
  uint8_t size;
  uint8_t max_size;
};

/*
 * Functions return 0 in case of failure
 */

void queue_init (volatile struct queue_t *buf,
                 volatile uint8_t *data,
                 uint8_t max_size,
                 uint8_t pattern);

uint8_t queue_is_fillable (volatile struct queue_t *buf, uint8_t fill_size);
uint8_t queue_is_drainable (volatile struct queue_t *buf, uint8_t drain_size);

uint8_t queue_get (volatile struct queue_t *buf, uint8_t index,
                   volatile uint8_t **data);

uint8_t queue_fill_symbol (volatile struct queue_t *buf, uint8_t symbol);
uint8_t queue_drain_symbol (volatile struct queue_t *buf, uint8_t *symbol);

uint8_t queue_drain (volatile struct queue_t *buf, uint8_t drain_size);

/* clear old data and fill new data from src */
uint8_t queue_refill (volatile struct queue_t *buf,
                      volatile uint8_t *src, uint8_t src_size);

uint8_t queue_move (volatile struct queue_t *from, volatile struct queue_t *to);

void queue_clear (volatile struct queue_t *queue);

uint8_t queue_size (volatile struct queue_t *queue);

#endif
