/*
 *
 */
#ifndef RING_H
#define RING_H

#include <stdint.h>

/*
 * Functions return 0 in case of failure
 */

void ring_init (volatile uint8_t *buf, uint8_t max_size);

uint8_t ring_size (volatile uint8_t *buf);

/*make rin empty*/
void ring_clear ();

/*return free space in the ring buffer*/
uint8_t ring_space (volatile uint8_t *ring);

/* implement with ring-init */
/* uint8_t ring_clear (volatile uint8_t *buf); */

uint8_t ring_symbol_fill (volatile uint8_t *buf, uint8_t symbol);
uint8_t ring_symbol_drain (volatile uint8_t *buf, uint8_t *symbol);
uint8_t ring_symbol_get (volatile uint8_t *ring, uint8_t index, uint8_t *symbol);

/*fill ring from array*/
uint8_t ring_array_fill (volatile uint8_t *ring,
                         uint8_t *arr, uint8_t fill_size);
/*move data from ring to plain array (NB: not from ring to ring)*/
uint8_t ring_array_drain (volatile uint8_t *ring,
                          uint8_t *arr, uint8_t drain_size);

uint8_t ring_is_fillable (volatile uint8_t *buf, uint8_t fill_size);
uint8_t ring_is_drainable (volatile uint8_t *buf, uint8_t drain_size);



/* suspicious, access to internals
uint8_t ring_get (volatile uint8_t *buf, uint8_t index, volatile uint8_t **data);
*/
/* /\* clear old data and fill new data from src *\/ */
/* uint8_t ring_refill (volatile uint8_t *buf, uint8_t *src, uint8_t src_size); */
/* uint8_t ring_move (volatile uint8_t *to, volatile uint8_t *from); */
/* void ring_clear (volatile uint8_t *buf); */

#endif
