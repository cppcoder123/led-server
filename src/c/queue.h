/*
 *
 */
#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

/*
 * Functions return 0 in case of failure
 */

void queue_init (volatile uint8_t *queue, uint8_t max_size);

uint8_t queue_size (volatile uint8_t *queue);

/*make queue empty*/
void queue_clear (volatile uint8_t *queue);

/*return free space in the queue buffer*/
uint8_t queue_space (volatile uint8_t *queue);

uint8_t queue_symbol_fill (volatile uint8_t *queue, uint8_t symbol);
uint8_t queue_symbol_drain (volatile uint8_t *queue, uint8_t *symbol);
uint8_t queue_symbol_get (volatile uint8_t *queue, uint8_t index, uint8_t *symbol);

/*fill queue from array*/
uint8_t queue_array_fill (volatile uint8_t *queue,
                         uint8_t *arr, uint8_t fill_size);
/*move data from queue to plain array (NB: not from queue to queue)*/
uint8_t queue_array_drain (volatile uint8_t *queue,
                          uint8_t *arr, uint8_t drain_size);

uint8_t queue_is_fillable (volatile uint8_t *queue, uint8_t fill_size);
uint8_t queue_is_drainable (volatile uint8_t *queue, uint8_t drain_size);

#endif
