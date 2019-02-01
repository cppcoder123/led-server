/*
 *
 */
#include "device-id.h"

#include "codec.h"
#include "queue.h"
#include "uart-write.h"

volatile struct queue_t *write;

void codec_init ()
{
  write = uart_write_get_queue ();
}

void codec_encode_1 (uint8_t msg_id, uint8_t msg_serial_id, uint8_t data)
{
  /*
   * check space first:
   * eye-catcher (1), size (1), serial (1), msg-id (1), data (1)
   */
  if (queue_is_fillable (write, ID_HEADER_SIZE + 1) == 0) {
    /* how we should complain here? */
    return;
  }

  queue_fill_symbol (write, ID_EYE_CATCH);
  queue_fill_symbol (write, 1);
  queue_fill_symbol (write, msg_serial_id);
  queue_fill_symbol (write, msg_id);
  queue_fill_symbol (write, data);

  uart_write_kick ();
}

uint8_t codec_decode_header (volatile const uint8_t *src,
                             volatile uint8_t *msg_size,
                             volatile uint8_t *msg_id,
                             volatile uint8_t *msg_serial_id)
{
  if (*src != ID_EYE_CATCH)
    return 0;

  *msg_size = *(src + 1);
  *msg_serial_id = *(src + 2);
  *msg_id = *(src + 3);

  return 1;
}
