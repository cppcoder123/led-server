/*
 *
 */
#include <avr/interrupt.h>
#include <stdint.h>

#include "mcu/constant.h"

#include "debug.h"
#include "encode.h"
//#include "ring.h"
#include "keyboard.h"
#include "twi.h"

#define KEY_WAKEUP 0x01
#define KEY_REG_2 0x40

// state of our buttons
static volatile uint8_t state = 0;
static uint8_t prev_state = 0;

/* mode of operation */
enum {
    mode_started,
    mode_reading,
    mode_finished
};
static volatile uint8_t mode = mode_started;


void keyboard_init ()
{
  state = 0;
  prev_state = 0;

  mode = mode_started;
  

  // fixme
}

/* measure then read cycle */

static uint8_t cycle_finished ()
{
    return (mode == mode_finished) ? 1 : 0;
}

static void cycle_start ()
{
  mode = mode_started;

  twi_write_reg (KEY_REG_2, KEY_WAKEUP, xxx);
  
  // fixme
}

void keyboard_try ()
{
  if (cycle_finished () == 0)
    return;

  if (state != prev_state) {
    encode_msg_2 (MSG_ID_BUTTON, SERIAL_ID_TO_IGNORE, state, prev_state);
    prev_state = state;
  }

  cycle_start ();
}
