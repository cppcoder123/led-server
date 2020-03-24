/*
 *
 */
#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

#define BUFFER_SIZE 255

struct buffer_t
{
  uint8_t data[BUFFER_SIZE];
  uint8_t start;
  uint8_t finish;
};

/*
 * Functions return 0 in case of failure
 */

void buffer_init (struct buffer_t *buffer);

uint8_t buffer_size (struct buffer_t *buffer);

/* make it empty */
void buffer_clear (struct buffer_t *buffer);

/* return free space */
uint8_t buffer_space (struct buffer_t *buffer);

uint8_t buffer_byte_fill (struct buffer_t *buffer, uint8_t byte);
uint8_t buffer_byte_drain (struct buffer_t *buffer, uint8_t *byte);
uint8_t buffer_byte_get (struct buffer_t *buffer,
                         uint8_t index, uint8_t *byte);

uint8_t buffer_array_fill (struct buffer_t *buffer,
                           uint8_t *array, uint8_t array_size);
uint8_t buffer_array_drain (struct buffer_t *buffer,
                            uint8_t *array, uint8_t array_size);

uint8_t buffer_is_fillable (struct buffer_t *buffer, uint8_t fill_size);
uint8_t buffer_is_drainable (struct buffer_t *buffer, uint8_t drain_size);


#endif
