/*
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "unix/constant.h"

#include "buf.h"
#include "encode.h"
#include "rotor.h"

#define STATE_DEFAULT 0xFF

static volatile struct buf_t rotor_buf;
static uint8_t state = STATE_DEFAULT;

void rotor_init ()
{
  buf_init (&rotor_buf);
  state = STATE_DEFAULT;

  /* enable 3-rd series of pin change interrupts */
  PCICR |= (1 << PCIE2);

  /* enable all 8 pins, though we need only 6 */
  PCMSK2 =  0xFF;

  /* enable internal pull-up resistors */
  PORTK = 0xFF;

  /*port K is input*/
  DDRK = 0;
}

void rotor_try ()
{
  uint8_t new_state = 0;
  if (buf_byte_drain (&rotor_buf, &new_state) == 0)
    return;

  /* fixme: deeper analysis is required here, */
  /*    i.e. which rotor and what button is pushed */

  /* report bits that changed to zero for now*/
  uint8_t to_send = 0;
  uint8_t mask = (1 << 0);
  for (uint8_t i = 0; i < 8; ++i) {
    if (((new_state & mask) == 0)
        && ((state & mask) != 0))
      to_send |= mask;
    mask <<= 1;
  }
  encode_msg_1 (MSG_ID_ROTOR, SERIAL_ID_TO_IGNORE, to_send);

  state = new_state;
}

ISR (PCINT2_vect)
{
  buf_byte_fill (&rotor_buf, PINK);
}
