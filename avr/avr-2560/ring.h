/*
 *
 */
#ifndef RING_H
#define RING_H

#include <stdint.h>

#include "data-type.h"

/*
 * Functions return 0 in case of failure
 */

void ring_init (volatile data_t *buf, uint8_t max_size);

uint8_t ring_size (volatile data_t *buf);

/*make rin empty*/
void ring_clear ();

/*return free space in the ring buffer*/
uint8_t ring_space (volatile data_t *ring);

/* implement with ring-init */
/* uint8_t ring_clear (volatile data_t *buf); */

uint8_t ring_symbol_fill (volatile data_t *buf, data_t symbol);
uint8_t ring_symbol_drain (volatile data_t *buf, data_t *symbol);
uint8_t ring_symbol_get (volatile data_t *ring, uint8_t index, data_t *symbol);

/*fill ring from array*/
uint8_t ring_array_fill (volatile data_t *ring,
                         data_t *arr, uint8_t fill_size);
/*move data from ring to plain array (NB: not from ring to ring)*/
uint8_t ring_array_drain (volatile data_t *ring,
                          data_t *arr, uint8_t drain_size);

uint8_t ring_is_fillable (volatile data_t *buf, uint8_t fill_size);
uint8_t ring_is_drainable (volatile data_t *buf, uint8_t drain_size);



/* suspicious, access to internals
uint8_t ring_get (volatile data_t *buf, uint8_t index, volatile data_t **data);
*/
/* /\* clear old data and fill new data from src *\/ */
/* uint8_t ring_refill (volatile data_t *buf, data_t *src, uint8_t src_size); */
/* uint8_t ring_move (volatile data_t *to, volatile data_t *from); */
/* void ring_clear (volatile data_t *buf); */

#endif
