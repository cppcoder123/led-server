/*
 *
 */
#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

/* Do not access directly use functions instead */
struct buffer_t
{
  uint8_t *data;
  uint8_t size;
  uint8_t max_size;
};

/*
 * Functions return 0 in case of failure
 */

void buffer_init (volatile struct buffer_t *buf,
                  uint8_t *data, uint8_t max_size);

uint8_t buffer_is_fillable (volatile struct buffer_t *buf, uint8_t fill_size);
uint8_t buffer_is_drainable (volatile struct buffer_t *buf, uint8_t drain_size);

uint8_t buffer_get (volatile struct buffer_t *buf, uint8_t index, uint8_t **data);

uint8_t buffer_fill_symbol (volatile struct buffer_t *buf, uint8_t symbol);
uint8_t buffer_drain_symbol (volatile struct buffer_t *buf, uint8_t *symbol);

uint8_t buffer_drain (volatile struct buffer_t *buf, uint8_t drain_size);

#endif
