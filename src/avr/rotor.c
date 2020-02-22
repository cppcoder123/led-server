/*
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "at.h"
#include "buf.h"
#include "debug.h"
#include "rotor.h"

/* no more than 2 presses per second */
/* #define BOUNCE_DELAY 20 */
/* 1 sec for bouncing */
#define BOUNCE_DELAY 1

#define MASK_ROTOR_1_A (1 << 3)
#define MASK_ROTOR_1_B (1 << 5)
#define MASK_ROTOR_1_BOTH (MASK_ROTOR_1_A | MASK_ROTOR_1_B)
#define MASK_ROTOR_1_PUSH (1 << 7)

#define MASK_ROTOR_2_A (1 << 2)
#define MASK_ROTOR_2_B (1 << 4)
#define MASK_ROTOR_2_BOTH (MASK_ROTOR_2_A | MASK_ROTOR_2_B)
#define MASK_ROTOR_2_PUSH (1 << 6)

/*
 * It looks that turn buttons are bounce protected by hardware,
 * but push button doesn't.
 * So, try to implement software debouncing.
 */
#define BOUNCE_1 (1 << 0)
#define BOUNCE_2 (1 << 1)

static volatile struct buf_t event_buf;
static uint8_t toggled_bits = 0;
static uint8_t bounce = 0;

static rotor_callback callback = 0;

static void dummy_callback (uint8_t id, uint8_t action)
{
}

void rotor_init ()
{
  buf_init (&event_buf);
  toggled_bits = 0;
  bounce = 0;
  callback = &dummy_callback;
  /* invoke_enable (INVOKE_ID_ROTOR, BOUNCE_DELAY, &bounce_clear); */

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

static void bounce_clear_1 ()
{
  bounce &= ~BOUNCE_1;
}

static void bounce_clear_2 ()
{
  bounce &= ~BOUNCE_2;
}

static void debounce (uint8_t id)
{
  uint8_t mask = (id == ROTOR_1) ? BOUNCE_1 : BOUNCE_2;

  if ((bounce & mask) != 0)
    return;

  bounce |= mask;

  if (id == ROTOR_1)
    at_schedule (AT_BOUNCE_1, BOUNCE_DELAY, &bounce_clear_1);
  else
    at_schedule (AT_BOUNCE_2, BOUNCE_DELAY, &bounce_clear_2);

  callback (id, ROTOR_PUSH);
}

void rotor_try ()
{
  uint8_t src = 0;
  if (buf_byte_drain (&event_buf, &src) == 0)
    return;

  /* negate src */
  uint8_t new_toggled_bits = ~src;

  /* debug_1 (DEBUG_ROTOR, DEBUG_11, new_toggled_bits); */

  if (apply_mask
      (toggled_bits, new_toggled_bits, MASK_ROTOR_1_A, MASK_ROTOR_1_BOTH) != 0)
    callback (ROTOR_1, ROTOR_CLOCKWISE);
  if (apply_mask
      (toggled_bits, new_toggled_bits, MASK_ROTOR_1_B, MASK_ROTOR_1_BOTH) != 0)
    callback (ROTOR_1, ROTOR_COUNTER_CLOCKWISE);
  if (new_toggled_bits & MASK_ROTOR_1_PUSH)
    debounce (ROTOR_1);

  if (apply_mask
      (toggled_bits, new_toggled_bits, MASK_ROTOR_2_A, MASK_ROTOR_2_BOTH) != 0)
    callback (ROTOR_2, ROTOR_COUNTER_CLOCKWISE);
  if (apply_mask
      (toggled_bits, new_toggled_bits, MASK_ROTOR_2_B, MASK_ROTOR_2_BOTH) != 0)
    callback (ROTOR_2, ROTOR_CLOCKWISE);
  if (new_toggled_bits & MASK_ROTOR_2_PUSH)
    debounce (ROTOR_2);

  toggled_bits = new_toggled_bits;
}

ISR (PCINT2_vect)
{
  buf_byte_fill (&event_buf, PINK); /* floyd */
}

void rotor_register (rotor_callback cb)
{
  if (cb == 0)
    return;

  callback = cb;
}
