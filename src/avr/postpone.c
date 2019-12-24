/*
 *
 */
#include <util/atomic.h>

#include "unix/constant.h"

#include "flush.h"
#include "postpone.h"
#include "spi.h"

#define BUF_SIZE 20

#define ENOUGH_SPACE (LED_ARRAY_SIZE + MSG_OVERHEAD)

static uint8_t postponed_message[BUF_SIZE];
static uint8_t postponed_size;

static uint8_t is_enough_space ()
{
  return ((spi_read_space () >= ENOUGH_SPACE)
          && (spi_write_space () >= ENOUGH_SPACE)
          && (flush_buffer_space () >= ENOUGH_SPACE)) ? 1 : 0;
}

static void flush (uint8_t *msg, uint8_t msg_size)
{
  spi_write_array (msg, msg_size);
}

void postpone_init ()
{
  postponed_size = 0;
  for (uint8_t i = 0; i < BUF_SIZE; ++i)
    postponed_message[i] = SPI_WRITE_UNDERFLOW;
}

void postpone_try ()
{
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if ((postponed_size == 0)
        || (is_enough_space () == 0))
      return;

    flush (postponed_message, postponed_size);
    postponed_size = 0;
  }
}

void postpone_message (uint8_t *msg, uint8_t msg_size, uint8_t serial_id)
{
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    if (serial_id == SERIAL_ID_TO_IGNORE) {
      flush (msg, msg_size);
      return;
    }

    /* simplify possible debug */
    if (postponed_size != 0)
      return;

    postponed_size = msg_size;
    for (uint8_t i = 0; i < msg_size; ++i)
      postponed_message[i] = msg[i];
  }
}
