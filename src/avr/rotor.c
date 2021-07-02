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

static uint8_t handle_event (uint8_t source, uint8_t modern, uint8_t old, uint8_t oldest)
{
  uint8_t mask_a = MASK_A;
  uint8_t mask_b = MASK_B;
  uint8_t mask_both = (mask_a | mask_b);

  const uint8_t shift = (source == ID_J) ? ROTOR_4 : ROTOR_0;
  uint8_t status = 0;

  for (uint8_t i = ROTOR_0; i < ROTOR_4; ++i) {
    if ((modern & mask_both) == 0) {
      if ((old & mask_a)
          && (oldest & mask_b)) {
        menu_handle_rotor (i + shift, ROTOR_COUNTER_CLOCKWISE);
        status = 1;
      } else if ((old & mask_b)
                 && (oldest & mask_a)) {
        menu_handle_rotor (i + shift, ROTOR_CLOCKWISE);
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

static uint8_t queue_head (struct buf_t *buf,
                           uint8_t *modern, uint8_t *old, uint8_t *oldest)
{
  if (buf_size (buf) < 3)
    return 0;

  return ((buf_byte_get (buf, 0, oldest))
          && (buf_byte_get (buf, 1, old))
          && (buf_byte_get (buf, 2, modern)));
}

static void queue_drain (struct buf_t *buf, uint8_t drain_size)
{
  uint8_t dummy = 0;
  for (uint8_t i = 0; i < drain_size; ++i)
    buf_byte_drain (buf, &dummy);
}

static void try_queue (struct buf_t *buf, uint8_t queue_id)
{
  /* current, previous and before previous events*/
  uint8_t modern = 0, old = 0, oldest = 0;
  if (queue_head (buf, &modern, &old, &oldest)) {
      if (handle_event (queue_id, modern, old, oldest))
        /*
         * Succeded to drain  the queue,
         * and succeded to handle it, drain all 3 events
         */
        queue_drain (buf, 3);
      else
        /*        
         * Queue handling is failed => bouncing ?
         * Discard the oldest event
         */
        queue_drain (buf, 1);
  }
}

void rotor_try ()
{
  try_queue (&j_event_buf, ID_J);
  try_queue (&k_event_buf, ID_K);
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
