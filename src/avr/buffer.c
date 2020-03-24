/*
 *
 */

#include "buffer.h"
#include "ring.h"

void buffer_init (struct buffer_t *buffer)
{
  ring_init (BUFFER_SIZE, buffer->data, &buffer->start, &buffer->finish);
}

uint8_t buffer_size (struct buffer_t *buffer)
{
  return ring_size (BUFFER_SIZE, buffer->start, buffer->finish);
}

void buffer_clear (struct buffer_t *buffer)
{
  ring_clear (&buffer->start, &buffer->finish);
}

uint8_t buffer_space (struct buffer_t *buffer)
{
  return ring_space (BUFFER_SIZE, buffer->start, buffer->finish);
}

uint8_t buffer_byte_fill (struct buffer_t *buffer, uint8_t byte)
{
  return ring_byte_fill (BUFFER_SIZE, buffer->data,
                         buffer->start, &buffer->finish, byte);
}

uint8_t buffer_byte_drain (struct buffer_t *buffer, uint8_t *byte)
{
  return ring_byte_drain (BUFFER_SIZE, buffer->data,
                          &buffer->start, &buffer->finish, byte);
}

uint8_t buffer_byte_get (struct buffer_t *buffer,
                         uint8_t index, uint8_t *byte)
{
  return ring_byte_get (BUFFER_SIZE, buffer->data,
                        buffer->start, buffer->finish, index, byte);
}

uint8_t buffer_array_fill (struct buffer_t *buffer,
                           uint8_t *array, uint8_t array_size)
{
  return ring_array_fill (BUFFER_SIZE, buffer->data,
                          buffer->start, &buffer->finish, array, array_size);
}

uint8_t buffer_array_drain (struct buffer_t *buffer,
                            uint8_t *array, uint8_t array_size)
{
  return ring_array_drain (BUFFER_SIZE, buffer->data,
                           &buffer->start, &buffer->finish, array, array_size);
}

uint8_t buffer_is_fillable (struct buffer_t *buffer, uint8_t fill_size)
{
  return ring_is_fillable (BUFFER_SIZE, buffer->start, buffer->finish, fill_size);
}

uint8_t buffer_is_drainable (struct buffer_t *buffer, uint8_t drain_size)
{
  return ring_is_drainable (BUFFER_SIZE, buffer->start, buffer->finish, drain_size);
}
