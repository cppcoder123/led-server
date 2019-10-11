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

#define BOARD_ERROR_SHIFT_100 100

#define BOARD_WAKEUP 0x01
#define BOARD_DETECT_SUCCESS 0  /* fixme */

#define BOARD_REG_1 0x1C
#define BOARD_REG_2 0x40
#define BOARD_REG_STATUS 0x1A
#define BOARD_REG_DATA 0x19
#define BOARD_REG_OFFCAL_CYCLE 0x22

#define BOARD_VALUE_0X80 0x80
#define BOARD_VALUE_0X01 0x01
#define BOARD_VALUE_0X1C 0x1C

// state of our buttons
static volatile uint8_t state = 0;
static uint8_t prev_state = 0;

/* mode of operation */
enum {
  mode_error,                   /* smth bad happened */
  mode_reset,                   /* reset */
  mode_calibrate_start,         /* self calibration start */
  mode_calibrate_wait_start,    /* self calibration wait */
  mode_calibrate_wait_check,    /* check calibrate done condition */
  mode_calibrate_param,         /* write one calibration param */
  mode_calibrate_apply,         /* apply calibration values */
  mode_wakeup_write,            /* write wakeup command */
  mode_wakeup_wait,             /* wait the interrupt */
  mode_status_read,             /* finger detection status read */
  mode_status_check,            /* finger detection status check */
  mode_data_read,               /* finger detection data read */
  mode_data_handle,             /* finger detection data handle */
  mode_idle,                    /* ready for next cycle */
  mode_last = mode_idle         /* last mode */
};
static volatile uint8_t mode = mode_reset;

static volatile uint8_t mode_advance = 0;

static void board_reset ()
{
  //fixme
  // make low pulse here on reset line
}

void keyboard_init ()
{
  state = 0;
  prev_state = 0;

  // fixme:
  // init ext interrupt request line here

  mode = mode_reset;
  mode_advance = 0;
  board_reset ();
}

static void write_completed (uint8_t status)
{
  if (status != TWI_SUCCESS) {
    encode_msg_1 (MSG_ID_BOARD_WRITE_ERROR, SERIAL_ID_TO_IGNORE, status);
    mode = mode_error;
  }

  mode_advance = 1;
}

static void write_reg (uint8_t reg, uint8_t data)
{
  twi_write_reg (reg, data, write_completed);
}

static void read_completed (uint8_t status, uint8_t data)
{
  if (status != TWI_SUCCESS) {
    encode_msg_1 (MSG_ID_BOARD_READ_ERROR, SERIAL_ID_TO_IGNORE, status);
    mode = mode_error;
  }

  //fixme: store data in global var
  
  mode_advance = 1;
}

static void read_reg (uint8_t reg)
{
  twi_read_reg (reg, read_completed);
}

static uint8_t is_calibrated ()
{
  // fixme
  return 1;
}

static uint8_t good_status ()
{
  // fixme
  return 1;
}

static void handle_data ()
{
  // fixme
}

void keyboard_try ()
{
  if (mode_advance == 0)
    return;

  uint8_t mode_tmp = mode;

  if (mode < mode_last)
    ++mode;

  mode_advance = 0;
  
  switch (mode_tmp) {
  case mode_error:
    board_reset ();
    break;
  case mode_reset:
    mode_advance = 1;           /* nothing to do */
    break;
  case mode_calibrate_start:
    write_reg (BOARD_REG_1, BOARD_VALUE_0X80);
    break;
  case mode_calibrate_wait_start:
    read_reg (BOARD_REG_1);
    break;
  case mode_calibrate_wait_check:
    mode_advance = 1;
    if (is_calibrated () == 0)
      mode -= 2;                /* one step backward */
    break;
  case mode_calibrate_param:
    write_reg (BOARD_REG_OFFCAL_CYCLE, BOARD_VALUE_0X01);
    break;
  case mode_calibrate_apply:
    write_reg (BOARD_REG_1, BOARD_VALUE_0X1C);
    break;
  case mode_wakeup_write:
    write_reg (BOARD_REG_2, BOARD_WAKEUP);
    break;
  case mode_wakeup_wait:
    // waiting for interrupt here
    break;
  case mode_status_read:
    read_reg (BOARD_REG_STATUS);
    break;
  case mode_status_check:
    if (good_status () == 0) {
      mode = mode_error;
    }
    mode_advance = 1;
    break;
  case mode_data_read:
    read_reg (BOARD_REG_DATA);
    break;
  case mode_data_handle:
    mode_advance = 1;
    handle_data ();
    break;
  case mode_idle:
    mode_advance = 1;
    mode = mode_wakeup_write;
    break;
  default:
    mode_advance = 1;
    mode = mode_error;
    encode_msg_1 (MSG_ID_BOARD_HANDLE_ERROR, SERIAL_ID_TO_IGNORE, mode);
    break;
  }

}

/* handle external interrupt from board */
ISR (INT2_vect)
{
  mode_advance = 1;
}
