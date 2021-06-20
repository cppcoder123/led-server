/*
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "const/constant.h"

#include "at.h"
#include "buf.h"
#include "debug.h"
#include "rotor.h"
#include "menu.h"

/*all bits of K is used*/
#define MASK_K (0xFF)
/*0,1 bits of J*/
#define MASK_J ((1 << 0) | (1 << 1))

/*clockwise & counter clockwise*/
#define MASK_CLOCKWISE (1 << 0)
#define MASK_COUNTER_CLOCKWISE (1 << 1)

/* a & b inputs */
#define MASK_A (1 << 0)
#define MASK_B (1 << 1)

/*aux id to note interrupt source and put it into the queue*/
enum {
      ID_J,
      ID_K,
};

/* one knob takes 2 bits, we are ignoring push*/
#define STEP 2

static struct buf_t j_event_buf;
static struct buf_t k_event_buf;

void rotor_init ()
{
  buf_init (&j_event_buf);
  buf_init (&k_event_buf);
  /* old_event_j = 0; */
  /* old_event_k = 0; */

  /*Ports K(0-7) & J(0,1) are inputs*/
  DDRK &= ~MASK_K;
  DDRJ &= ~MASK_J;

  /* enable 2nd and 3-rd series of pin change interrupts */
  PCICR |= ((1 << PCIE1) | (1 << PCIE2));

  /* set pin change mask to needed bits */
  PCMSK1 |= (1 << 1) | (1 << 2);
  PCMSK2 |= MASK_K;

  /* enable internal pull-up resistors */
  PORTK |= MASK_K;
  PORTJ |= MASK_J;
}

static uint8_t handle_event (uint8_t source, uint8_t event, uint8_t prev_event)
{
  uint8_t mask_a = MASK_A;
  uint8_t mask_b = MASK_B;
  uint8_t mask_both = (mask_a | mask_b);

  const uint8_t shift = (source == ID_J) ? ROTOR_4 : ROTOR_0;
  uint8_t status = 0;

  for (uint8_t i = ROTOR_0; i < ROTOR_4; ++i) {
    if ((event & mask_both) == 0) {
      if (prev_event & mask_a) {
        menu_handle_rotor (i + shift, ROTOR_CLOCKWISE);
        status = 1;
      } else if (prev_event & mask_b) {
        menu_handle_rotor (i + shift, ROTOR_COUNTER_CLOCKWISE);
        status = 1;
      }
      if (source == ID_J)
        break;
    }
    mask_a <<= STEP;
    mask_b <<= STEP;
    mask_both = mask_a | mask_b;
  }

  return status;
}

static uint8_t drain_queue (struct buf_t *buf,
                            uint8_t *event, uint8_t *prev_event)
{
  if (buf_size (buf) < 2)
    return 0;

  return ((buf_byte_drain (buf, prev_event) != 0)
          && (buf_byte_drain (buf, event) != 0)) ? 1 : 0;
}

void rotor_try ()
{
  uint8_t event = 0;
  uint8_t prev_event = 0;
  if ((drain_queue (&j_event_buf, &event, &prev_event)) /* succeded to drain */
      && (handle_event (ID_J, event, prev_event) == 0)) /* failed to handle */
    /* we need to keep event as prev event for next cycle */
    buf_byte_fill (&j_event_buf, event);

  if ((drain_queue (&k_event_buf, &event, &prev_event))
      && (handle_event (ID_K, event, prev_event) == 0))
    buf_byte_fill (&k_event_buf, event);
}

ISR (PCINT1_vect)
{
  /* debug_2 (DEBUG_ROTOR, 11, 11, ~(PINJ & MASK_J) & MASK_J); */
  buf_byte_fill (&j_event_buf, ~(PINJ & MASK_J) & MASK_J);
}

ISR (PCINT2_vect)
{
  /* debug_2 (DEBUG_ROTOR, 22, 22, ~(PINK & MASK_K)); */
  buf_byte_fill (&k_event_buf, ~(PINK & MASK_K));
}
