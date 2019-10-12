/*
 *
 */

#define F_CPU 4000000UL

#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

#include "mcu/constant.h"

#include "debug.h"
#include "encode.h"
#include "keyboard.h"
#include "twi.h"

#define BOARD_ERROR_SHIFT_100 100

#define BOARD_REG_CONTROL_1 0x1C
#define BOARD_REG_CONTROL_2 0x40
#define BOARD_REG_GENERAL_STATUS 0x1A
#define BOARD_REG_CHANNEL_STATUS 0x1B
#define BOARD_REG_DATA 0x19
#define BOARD_REG_OFFCAL_CYCLE 0x22

#define BOARD_VALUE_WAKEUP 0x01
#define BOARD_VALUE_CALIBRATE 0x80
#define BOARD_VALUE_CALIBRATE_PARAM 0x01
#define BOARD_VALUE_CALIBRATE_APPLY 0x87
/* it doesn't matter what we have for x */
/* 0xxx x000 (negate=>) 1xxx x111 => 1000 0111 => 0x87*/
#define BOARD_VALUE_CALIBRATED_MASK 0x86
/* MSB and LSB to 1 */
#define BOARD_VALUE_GENERAL_STATUS_MASK 0x81
/* all bits should be zero */
#define BOARD_VALUE_CHANNEL_STATUS_MASK 0xFF

#define PORT_RESET PORTC0

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
  mode_general_status_read,     /* measurement general status read */
  mode_general_status_check,    /* measurement general status check */
  mode_channel_status_read,     /* measurement channel status read */
  mode_channel_status_check,    /* measurement channel status check */
  mode_data_read,               /* measurement data read */
  mode_data_handle,             /* measurement data handle */
  mode_idle,                    /* ready for next cycle */
  mode_last = mode_idle         /* last mode */
};
static uint8_t mode = mode_reset;
static volatile uint8_t mode_advance = 0;

/* state of our buttons */
static uint8_t state = 0;

/* read register value */
static uint8_t read_value = 0;

static void board_reset ()
{
  /* set portc 0th bit to 0, then to 1 */
  PORTC &= ~(1 << PORT_RESET);
  _delay_us (10);
  PORTC |= (1 << PORT_RESET);
}

void keyboard_init ()
{
  mode = mode_reset;
  mode_advance = 0;

  state = 0;

  read_value = 0;

  EICRA |= (1 << ISC20) | (1 << ISC21); /* rising edge */
  EIMSK |= (1 << INT2);                 /* enable int2 interrupt */

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

  read_value = data;
  
  mode_advance = 1;
}

static void read_reg (uint8_t reg)
{
  twi_read_reg (reg, read_completed);
}

static uint8_t check_zero (uint8_t mask)
{
  return ((read_value & mask) == 0) ? 1 : 0;
}

static void handle_data (uint8_t new_state)
{
  if (new_state == state)
    return;

  encode_msg_2 (MSG_ID_BUTTON, SERIAL_ID_TO_IGNORE, new_state, state);

  state = new_state;
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
    write_reg (BOARD_REG_CONTROL_1, BOARD_VALUE_CALIBRATE);
    break;
  case mode_calibrate_wait_start:
    read_reg (BOARD_REG_CONTROL_1);
    break;
  case mode_calibrate_wait_check:
    if (check_zero (BOARD_VALUE_CALIBRATED_MASK) == 0)
      mode -= 2;                /* one step backward */
    break;
  case mode_calibrate_param:
    write_reg (BOARD_REG_OFFCAL_CYCLE, BOARD_VALUE_CALIBRATE_PARAM);
    break;
  case mode_calibrate_apply:
    write_reg (BOARD_REG_CONTROL_1, BOARD_VALUE_CALIBRATE_APPLY);
    break;
  case mode_wakeup_write:
    write_reg (BOARD_REG_CONTROL_2, BOARD_VALUE_WAKEUP);
    break;
  case mode_wakeup_wait:
    // waiting for interrupt here
    mode_advance = 1;
    break;
  case mode_general_status_read:
    read_reg (BOARD_REG_GENERAL_STATUS);
    break;
  case mode_general_status_check:
    if (check_zero (BOARD_VALUE_GENERAL_STATUS_MASK) == 0) {
      mode = mode_error;
    }
    mode_advance = 1;
    break;
  case mode_channel_status_read:
    read_reg (BOARD_REG_CHANNEL_STATUS);
    break;
  case mode_channel_status_check:
    if (check_zero (BOARD_VALUE_CHANNEL_STATUS_MASK) == 0) {
      mode = mode_error;
    }
    mode_advance = 1;
    break;
  case mode_data_read:
    read_reg (BOARD_REG_DATA);
    break;
  case mode_data_handle:
    mode_advance = 1;
    handle_data (read_value);
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
