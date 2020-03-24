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
#include "menu.h"

/*
 * 2 sec for bouncing
 *
 * Actually bouncing should be around 0.05-0.1 sec
 * but 'at' resolution is 1 second and
 * with 1 second it doesn't work well.
 * Perhaps 1 second is not enough because it is min avalaible value.
 * So, max rate is aprox 1 push per 2 seconds.
 */
#define BOUNCE_DELAY 2

#define MASK_ROTOR_0_A (1 << 3)
#define MASK_ROTOR_0_B (1 << 5)
#define MASK_ROTOR_0_BOTH (MASK_ROTOR_0_A | MASK_ROTOR_0_B)
#define MASK_ROTOR_0_PUSH (1 << 1)

#define MASK_ROTOR_1_A (1 << 2)
#define MASK_ROTOR_1_B (1 << 4)
#define MASK_ROTOR_1_BOTH (MASK_ROTOR_1_A | MASK_ROTOR_1_B)
#define MASK_ROTOR_1_PUSH (1 << 0)

/*
 * It looks that turn buttons are bounce protected by hardware,
 * but push button doesn't.
 * So, try to implement software debouncing.
 */
#define BOUNCE_0 (1 << 0)
#define BOUNCE_1 (1 << 1)

static struct buf_t event_buf;
static uint8_t old_event = 0;       /* toggled bits */
static uint8_t bounce = 0;

/* static rotor_callback menu_handle_rotor = 0; */

/* static void dummy_callback (uint8_t id, uint8_t action) */
/* { */
/* } */

void rotor_init ()
{
  buf_init (&event_buf);
  old_event = 0;
  bounce = 0;
  /* menu_handle_rotor = &dummy_callback; */
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

static void bounce_clear_0 ()
{
  bounce &= ~BOUNCE_0;
}

static void bounce_clear_1 ()
{
  bounce &= ~BOUNCE_1;
}

static void debounce (uint8_t id)
{
  uint8_t mask = (id == ROTOR_0) ? BOUNCE_0 : BOUNCE_1;

  if ((bounce & mask) != 0)
    return;

  bounce |= mask;

  if (id == ROTOR_0)
    at_schedule (AT_BOUNCE_0, BOUNCE_DELAY, &bounce_clear_0);
  else
    at_schedule (AT_BOUNCE_1, BOUNCE_DELAY, &bounce_clear_1);

  menu_handle_rotor (id, ROTOR_PUSH);
}

static uint8_t apply_mask (uint8_t old, uint8_t new,
                           uint8_t old_mask, uint8_t new_mask)
{
  return (((old & old_mask) == old_mask)
          && ((new & new_mask) == new_mask)) ? 1 : 0;
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

  if (apply_mask (old_event, event, mask_a, mask_both) != 0)
    menu_handle_rotor (id, ROTOR_CLOCKWISE);
  if (apply_mask (old_event, event, mask_b, mask_both) != 0)
    menu_handle_rotor (id, ROTOR_COUNTER_CLOCKWISE);
  if (((old_event & mask_push) == 0)
      && ((event & mask_push) == mask_push))
    debounce (id);
}

void rotor_try ()
{
  uint8_t src = 0;
  if (buf_byte_drain (&event_buf, &src) == 0)
    return;

  uint8_t event = ~src;

  /* debug_1 (DEBUG_ROTOR, DEBUG_11, new_toggled_bits); */

  handle_event (event, ROTOR_0);
  handle_event (event, ROTOR_1);

  old_event = event;
}

ISR (PCINT2_vect)
{
  buf_byte_fill (&event_buf, PINK); /* floyd */
}

/* void rotor_register (rotor_callback cb) */
/* { */
/*   if (cb == 0) */
/*     return; */

/*   menu_handle_rotor = cb; */
/* } */
