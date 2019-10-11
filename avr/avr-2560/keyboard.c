/*
 *
 */
#include <avr/interrupt.h>
#include <stdint.h>

#include "mcu/constant.h"

#include "debug.h"
#include "encode.h"
#include "keyboard.h"
#include "twi.h"

#define BOARD_WAKEUP 0x01
#define BOARD_NO_ERROR 0          /* fixme */

#define BOARD_REG_2 0x40
#define BOARD_REG_ERROR_STATUS 0x1A
#define BOARD_REG_RESULT_DATA 0x19

// state of our buttons
static volatile uint8_t state = 0;
static uint8_t prev_state = 0;

/* mode of operation */
enum {
  mode_init_start,              /* board init started */
  mode_init_finish,             /* board init finished */
  mode_detect_start,            /* finger detection started */
  mode_detect_finish,           /* finger detection completed */
  mode_read_error_start,        /* read detection error started */
  mode_read_error_finish,       /* read detection error completed */
  mode_read_data_start,         /* read detection data started */
  mode_read_data_finish,        /* read detection data completed */
  mode_idle                     /* ready for next cycle */
};
static volatile uint8_t mode = mode_init_start;

static void board_reset ()
{
  mode = mode_init_start;
  //fixme
  // make low pulse here on reset line
}

void keyboard_init ()
{
  state = 0;
  prev_state = 0;

  // fixme:
  // init ext interrupt request line here

  board_reset ();
}

static void write_completed (uint8_t status)
{
  if (status == TWI_SUCCESS)
    return;

  encode_msg_1 (MSG_ID_TWI_READ_ERROR, SERIAL_ID_TO_IGNORE, status);
}

static void read_completed (uint8_t status, uint8_t data)
{
  if (status != TWI_SUCCESS) {
    encode_msg_1 (MSG_ID_TWI_WRITE_ERROR, SERIAL_ID_TO_IGNORE, status);
    mode = mode_idle;
    return;
  }

  switch (mode) {
  case mode_read_error_start:
    if (data == BOARD_NO_ERROR) {
      mode = mode_read_error_finish;
    } else {
      encode_msg_1 (MSG_ID_KEY_ERROR, SERIAL_ID_TO_IGNORE, data);
      mode = mode_idle;
    }
    break;
  case mode_read_data_start:
    state = data;
    mode = mode_idle;
    break;
  default:
    mode = mode_idle;
    encode_msg_1 (MSG_ID_TWI_WRITE_ERROR, SERIAL_ID_TO_IGNORE, status);
    break;
  }
}

void keyboard_try ()
{
  switch (mode) {
  case mode_detect_start:
  case mode_read_error_start:
  case mode_read_data_start:
    break;
  case mode_detect_finish:
    mode = mode_read_error_start;
    twi_read_reg (BOARD_REG_ERROR_STATUS, read_completed);
    break;
  case mode_read_error_finish:
    mode = mode_read_data_start;
    twi_read_reg (BOARD_REG_RESULT_DATA, read_completed);
  case mode_read_data_finish:
    mode = mode_idle;
    break;
  case mode_idle:
    if (state != prev_state) {
      encode_msg_2 (MSG_ID_BUTTON, SERIAL_ID_TO_IGNORE, state, prev_state);
      prev_state = state;
    }
    mode = mode_detect_start;
    twi_write_reg (BOARD_REG_2, BOARD_WAKEUP, write_completed);
    break;
  default:
    encode_msg_1 (MSG_ID_KEY_HANDLE_ERROR, SERIAL_ID_TO_IGNORE, mode);
    break;
  }
}

// fixme: add ext intr handler here and its init in 'keyboard_init'
// it should update mode variable
