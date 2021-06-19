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

/* #define MASK_ROTOR_0_A (1 << 2) */
/* #define MASK_ROTOR_0_B (1 << 4) */
/* #define MASK_ROTOR_0_BOTH (MASK_ROTOR_0_A | MASK_ROTOR_0_B) */
/* #define MASK_ROTOR_0_PUSH (1 << 0) */
/* #define MASK_ROTOR_0 (MASK_ROTOR_0_A | MASK_ROTOR_0_B | MASK_ROTOR_0_PUSH) */

/* #define MASK_ROTOR_1_A (1 << 3) */
/* #define MASK_ROTOR_1_B (1 << 5) */
/* #define MASK_ROTOR_1_BOTH (MASK_ROTOR_1_A | MASK_ROTOR_1_B) */
/* #define MASK_ROTOR_1_PUSH (1 << 1) */
/* #define MASK_ROTOR_1 (MASK_ROTOR_1_A | MASK_ROTOR_1_B | MASK_ROTOR_1_PUSH) */

/* #define MASK_ROTOR (MASK_ROTOR_0 | MASK_ROTOR_1) */

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

static struct buf_t event_buf;
static uint8_t old_event_j = 0;       /* toggled bits */
static uint8_t old_event_k = 0;

void rotor_init ()
{
  buf_init (&event_buf);
  old_event_j = 0;
  old_event_k = 0;

  /*Ports K(0-7) & J(0,1) are inputs*/
  DDRK &= ~MASK_K;
  DDRJ &= ~MASK_J;

  /* enable 2nd and 3-rd series of pin change interrupts */
  PCICR |= ((1 << PCIE1) | (1 << PCIE2));

  /* set pin change mask to needed bits */
  PCMSK1 |= MASK_J;
  PCMSK2 |= MASK_K;

  /* enable internal pull-up resistors */
  PORTK |= MASK_K;
  PORTJ |= MASK_J;
}

static void handle_event (uint8_t event, uint8_t source)
{
  const uint8_t mask_old = (source == ID_J) ? MASK_J : MASK_K;
  const uint8_t old_event = (source == ID_J) ? old_event_j : old_event_k;
  if (old_event & mask_old)
    return;
  
  uint8_t mask_a = MASK_A;
  uint8_t mask_b = MASK_B;
  /* uint8_t mask_both = MASK_A | MASK_B; */

  const uint8_t shift = (source == ID_J) ? ROTOR_4 : ROTOR_0;

  for (uint8_t i = ROTOR_0; i < ROTOR_4; ++i) {
    if ((event & mask_a) == mask_a)
      menu_handle_rotor (i + shift, ROTOR_CLOCKWISE);
    else if ((event & mask_b) == mask_b)
      menu_handle_rotor (i + shift, ROTOR_COUNTER_CLOCKWISE);
    if (source == ID_J)
      break;
    mask_a <<= STEP;
    mask_b <<= STEP;
    /* mask_both = mask_a | mask_b; */
  }

  /* if (id != ROTOR_0) { */
  /*   mask_a = MASK_ROTOR_1_A; */
  /*   mask_b = MASK_ROTOR_1_B; */
  /*   mask_push = MASK_ROTOR_1_PUSH; */
  /* } */
  /* const uint8_t mask_both = (mask_a | mask_b); */

  /* if ((old_event & mask_both) == 0) { */
  /*   if ((event & mask_a) == mask_a) */
  /*     menu_handle_rotor (id, ROTOR_CLOCKWISE); */
  /*   else if ((event & mask_b) == mask_b) */
  /*     menu_handle_rotor (id, ROTOR_COUNTER_CLOCKWISE); */
  /* } */

  /* if (((old_event & mask_push) == 0) */
  /*       && ((event & mask_push) == mask_push)) */
  /*   menu_handle_rotor (id, ROTOR_PUSH); */
}

void rotor_try ()
{
  uint8_t event_source = 0;
  uint8_t event = 0;
  if ((buf_byte_drain (&event_buf, &event_source) == 0)
      || (buf_byte_drain (&event_buf, &event) == 0)
      || ((event_source != ID_J)
          && (event_source != ID_K)))
    return;

  handle_event (event, event_source);

  if (event_source == ID_J)
    old_event_j = event;
  else
    old_event_k = event;
}

ISR (PCINT1_vect)
{
  buf_byte_fill (&event_buf, ID_J);
  buf_byte_fill (&event_buf, (~PINJ) & MASK_J);
}

ISR (PCINT2_vect)
{
  buf_byte_fill (&event_buf, ID_K);
  buf_byte_fill (&event_buf, (~PINK) & MASK_K);
}
