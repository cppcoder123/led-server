/*
 *
 */
#ifndef BUFFER_H
#define BUFFER_H

/*
 * 3 macros should be available before including this file:
 *
 * BUFFER_NAME, BUFFER_MAX_SIZE, BUFFER_SIZE_TYPE
 *
 */


#include <stdint.h>

struct BUFFER_NAME
{
  uint8_t data[BUFFER_MAX_SIZE];
  BUFFER_SIZE_TYPE size;
  BUFFER_SIZE_TYPE current;
};

inline void buffer_init (struct BUFFER_NAME *buffer)
{
  buffer->size = 0;
  buffer->current = 0;
}


#endif
