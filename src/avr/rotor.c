/*
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "unix/constant.h"

#include "at.h"
#include "buf.h"
#include "debug.h"
#include "rotor.h"
#include "menu.h"

#define MASK_ROTOR_0_A (1 << 2)
#define MASK_ROTOR_0_B (1 << 4)
#define MASK_ROTOR_0_BOTH (MASK_ROTOR_0_A | MASK_ROTOR_0_B)
#define MASK_ROTOR_0_PUSH (1 << 0)
#define MASK_ROTOR_0 (MASK_ROTOR_0_A | MASK_ROTOR_0_B | MASK_ROTOR_0_PUSH)

#define MASK_ROTOR_1_A (1 << 3)
#define MASK_ROTOR_1_B (1 << 5)
#define MASK_ROTOR_1_BOTH (MASK_ROTOR_1_A | MASK_ROTOR_1_B)
#define MASK_ROTOR_1_PUSH (1 << 1)
#define MASK_ROTOR_1 (MASK_ROTOR_1_A | MASK_ROTOR_1_B | MASK_ROTOR_1_PUSH)

#define MASK_ROTOR (MASK_ROTOR_0 | MASK_ROTOR_1)

static struct buf_t event_buf;
static uint8_t old_event = 0;       /* toggled bits */

void rotor_init ()
{
  buf_init (&event_buf);
  old_event = 0;

  /*port K is input*/
  DDRK &= ~MASK_ROTOR;

  /* enable 3-rd series of pin change interrupts */
  PCICR |= (1 << PCIE2);

  /* set pin change mask to needed bits */
  PCMSK2 |= MASK_ROTOR;

  /* enable internal pull-up resistors */
  PORTK |= MASK_ROTOR;

}

static void handle_event (uint8_t event, uint8_t id)
{
  uint8_t mask_a = MASK_ROTOR_0_A;
  uint8_t mask_b = MASK_ROTOR_0_B;
  uint8_t mask_push = MASK_ROTOR_0_PUSH;

  if (id != ROTOR_0) {
    mask_a = MASK_ROTOR_1_A;
    mask_b = MASK_ROTOR_1_B;
    mask_push = MASK_ROTOR_1_PUSH;
  }
  const uint8_t mask_both = (mask_a | mask_b);

  if ((old_event & mask_both) == 0) {
    if ((event & mask_a) == mask_a)
      menu_handle_rotor (id, ROTOR_CLOCKWISE);
    else if ((event & mask_b) == mask_b)
      menu_handle_rotor (id, ROTOR_COUNTER_CLOCKWISE);
  }

  if (((old_event & mask_push) == 0)
        && ((event & mask_push) == mask_push))
    menu_handle_rotor (id, ROTOR_PUSH);
}

void rotor_try ()
{
  uint8_t event = 0;
  if (buf_byte_drain (&event_buf, &event) == 0)
    return;

  handle_event (event, ROTOR_0);
  handle_event (event, ROTOR_1);

  old_event = event;
}

ISR (PCINT2_vect)
{
  buf_byte_fill (&event_buf, (~PINK) & MASK_ROTOR);
}
