/*
 *
 */

#include "buf.h"
#include "ring.h"

void buf_init (volatile struct buf_t *buf)
{
  ring_init (BUF_SIZE, buf->data, &buf->start, &buf->finish);
}

uint8_t buf_size (volatile struct buf_t *buf)
{
  return ring_size (BUF_SIZE, buf->start, buf->finish);
}

void buf_clear (volatile struct buf_t *buf)
{
  ring_clear (&buf->start, &buf->finish);
}

uint8_t buf_space (volatile struct buf_t *buf)
{
  return ring_space (BUF_SIZE, buf->start, buf->finish);
}

uint8_t buf_byte_fill (volatile struct buf_t *buf, uint8_t byte)
{
  return ring_byte_fill (BUF_SIZE, buf->data,
                         buf->start, &buf->finish, byte);
}

uint8_t buf_byte_drain (volatile struct buf_t *buf, uint8_t *byte)
{
  return ring_byte_drain (BUF_SIZE, buf->data,
                          &buf->start, buf->finish, byte);
}

uint8_t buf_byte_get (volatile struct buf_t *buf,
                         uint8_t index, uint8_t *byte)
{
  return ring_byte_get (BUF_SIZE, buf->data,
                        buf->start, buf->finish, index, byte);
}

uint8_t buf_array_fill (volatile struct buf_t *buf,
                           uint8_t *array, uint8_t array_size)
{
  return ring_array_fill (BUF_SIZE, buf->data,
                          buf->start, &buf->finish, array, array_size);
}

uint8_t buf_array_drain (volatile struct buf_t *buf,
                            uint8_t *array, uint8_t array_size)
{
  return ring_array_drain (BUF_SIZE, buf->data,
                           &buf->start, buf->finish, array, array_size);
}

uint8_t buf_is_fillable (volatile struct buf_t *buf, uint8_t fill_size)
{
  return ring_is_fillable (BUF_SIZE, buf->start, buf->finish, fill_size);
}

uint8_t buf_is_drainable (volatile struct buf_t *buf, uint8_t drain_size)
{
  return ring_is_drainable (BUF_SIZE, buf->start, buf->finish, drain_size);
}
