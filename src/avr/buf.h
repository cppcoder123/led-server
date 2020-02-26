/*
 *
 */
#ifndef BUF_H
#define BUF_H

#include <stdint.h>

#define BUF_SIZE 64

struct buf_t
{
  uint8_t data[BUF_SIZE];
  uint8_t start;
  uint8_t finish;
};

/*
 * Functions return 0 in case of failure
 */

void buf_init (volatile struct buf_t *buf);

uint8_t buf_size (volatile struct buf_t *buf);

/* make it empty */
void buf_clear (volatile struct buf_t *buf);

/* return free space */
uint8_t buf_space (volatile struct buf_t *buf);

uint8_t buf_byte_fill (volatile struct buf_t *buf, uint8_t byte);
uint8_t buf_byte_drain (volatile struct buf_t *buf, uint8_t *byte);
uint8_t buf_byte_get (volatile struct buf_t *buf,
                      uint8_t index, uint8_t *byte);

uint8_t buf_array_fill (volatile struct buf_t *buf,
                        uint8_t *array, uint8_t array_size);
uint8_t buf_array_drain (volatile struct buf_t *buf,
                         uint8_t *array, uint8_t array_size);

uint8_t buf_is_fillable (volatile struct buf_t *buf, uint8_t fill_size);
uint8_t buf_is_drainable (volatile struct buf_t *buf, uint8_t drain_size);


#endif
