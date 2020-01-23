/*
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "unix/constant.h"

#include "buf.h"
#include "debug.h"
#include "encode.h"
#include "rotor.h"

#define MASK_ROTOR_1_A (1 << 3)
#define MASK_ROTOR_1_B (1 << 5)
#define MASK_ROTOR_1_BOTH (MASK_ROTOR_1_A | MASK_ROTOR_1_B)
#define MASK_ROTOR_1_PUSH (1 << 7)

#define MASK_ROTOR_2_A (1 << 2)
#define MASK_ROTOR_2_B (1 << 4)
#define MASK_ROTOR_2_BOTH (MASK_ROTOR_2_A | MASK_ROTOR_2_B)
#define MASK_ROTOR_2_PUSH (1 << 6)

static volatile struct buf_t rotor_buf;
static uint8_t toggled_bits = 0;

void rotor_init ()
{
  buf_init (&rotor_buf);
  toggled_bits = 0;

  /* enable 3-rd series of pin change interrupts */
  PCICR |= (1 << PCIE2);

  /* enable all 8 pins, though we need only 6 */
  PCMSK2 =  0xFF;

  /* enable internal pull-up resistors */
  PORTK = 0xFF;

  /*port K is input*/
  DDRK = 0;
}

static uint8_t apply_mask (uint8_t old, uint8_t new,
                           uint8_t old_mask, uint8_t new_mask)
{
  return (((old & old_mask) == old_mask)
          && ((new & new_mask) == new_mask)) ? 1 : 0;
}

static void send_msg (uint8_t rotor_id, uint8_t what)
{
  encode_msg_2 (MSG_ID_ROTOR, SERIAL_ID_TO_IGNORE, rotor_id, what);
}

void rotor_try ()
{
  uint8_t src = 0;
  if (buf_byte_drain (&rotor_buf, &src) == 0)
    return;

  /* negate src */
  uint8_t new_toggled_bits = ~src;

  debug_1 (DEBUG_ROTOR, DEBUG_11, new_toggled_bits);

  if (apply_mask
      (toggled_bits, new_toggled_bits, MASK_ROTOR_1_A, MASK_ROTOR_1_BOTH) != 0)
    send_msg (ROTOR_1, ROTOR_COUNTER_CLOCKWISE);
  if (apply_mask
      (toggled_bits, new_toggled_bits, MASK_ROTOR_1_B, MASK_ROTOR_1_BOTH) != 0)
    send_msg (ROTOR_1, ROTOR_CLOCKWISE);
  if (new_toggled_bits & MASK_ROTOR_1_PUSH)
    send_msg (ROTOR_1, ROTOR_PUSH);

  if (apply_mask
      (toggled_bits, new_toggled_bits, MASK_ROTOR_2_A, MASK_ROTOR_2_BOTH) != 0)
    send_msg (ROTOR_2, ROTOR_COUNTER_CLOCKWISE);
  if (apply_mask
      (toggled_bits, new_toggled_bits, MASK_ROTOR_2_B, MASK_ROTOR_2_BOTH) != 0)
    send_msg (ROTOR_2, ROTOR_CLOCKWISE);
  if (new_toggled_bits & MASK_ROTOR_2_PUSH)
    send_msg (ROTOR_2, ROTOR_PUSH);

  toggled_bits = new_toggled_bits;
}

ISR (PCINT2_vect)
{
  buf_byte_fill (&rotor_buf, PINK); /* floyd */
}
