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

/* registers we want to access */
#define REG_CONTROL_1 0x1C
#define REG_CONTROL_2 0x40
#define REG_USE_CHANNEL 0x00
#define REG_GAIN 0x01           /* first gain register, 1 per channel */
#define REG_THRESHOLD 0x09      /* first threshold register, 1 per channel */
#define REG_ERROR_STATUS 0x1A
#define REG_AVERAGE_COUNT 0x1D
#define REG_FILTER 0x1E
#define REG_DEBOUNCE_COUNT 0x1F
#define REG_OFFCAL_CYCLE 0x22
#define REG_OFFCAL_PLUS 0x23
#define REG_OFFCAL_MINUS 0x24
#define REG_CANCEL_COUNT_LOWER 0x25
#define REG_CANCEL_COUNT_HIGHER 0x26
#define REG_CDAC_BASE 0x3D
#define REG_MEASUREMENT_MODE 0x3E
#define REG_DATA 0x19  /* needed data should be here :) */

/* lc717 reset line driver */
#define PORT_RESET PORTC0

/* mode of operation */
enum {
  mode_error,                   /* smth bad happened */
  /* setup parameters */
  mode_reset,                   /* reset */
  mode_enable_start,            /* start calibration */
  mode_enable_read,             /* read 'start' status */
  mode_enable_check,            /* check 'start' status */
  mode_use_channel,             /* param */
  mode_gain,                    /* param */
  mode_threshold,               /* param */
  mode_average_count,           /* param */
  mode_filter,                  /* param */
  mode_debounce_count,          /* param */
  mode_offcal_cycle,            /* param */
  mode_offcal_plus,             /* param */
  mode_offcal_minus,            /* param */
  mode_cancel_count_lower,      /* param */
  mode_cancel_count_higher,     /* param */
  mode_cdac_base,               /* param */
  mode_measurument_mode,        /* param */
  mode_param_apply,             /* apply new params */
  /* main loop */
  mode_wakeup_write,            /* write wakeup command */
  mode_wakeup_wait,             /* wait the interrupt */
  mode_read_error_status,       /* read error */
  mode_check_error_status,      /* check error */
  mode_read_data,               /* read buttons states */
  mode_handle_data,             /* handle button data */
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
  _delay_ms (1);
  PORTC |= (1 << PORT_RESET);
}

void keyboard_init ()
{
  mode = mode_error;            /* it should be enabled first */
  mode_advance = 0;

  state = 0;

  read_value = 0;

  EICRA |= (1 << ISC20) | (1 << ISC21); /* rising edge */
  EIMSK |= (1 << INT2);                 /* enable int2 interrupt */

  /* PORT_RESET drives reset line, configure it as output, write 1 */
  DDRC |= (1 << PORT_RESET);
  PORTC |= (1 << PORT_RESET);
}

static void write_completed (uint8_t status)
{
  if (status != TWI_SUCCESS) {
    encode_msg_1 (MSG_ID_BOARD_WRITE_ERROR, SERIAL_ID_TO_IGNORE, status);
    mode = mode_error;
  } else {
    mode_advance = 1;
  }
}

static void write_byte (uint8_t reg, uint8_t data)
{
  if (twi_write_byte (reg, data, write_completed) == 0)
    encode_msg_1 (MSG_ID_DEBUG_P, SERIAL_ID_TO_IGNORE, 222);
}

static void write_word (uint8_t reg, uint8_t data)
{
  twi_write_word (reg, data, write_completed);
}

static void read_completed (uint8_t status, uint8_t data)
{
  if (status != TWI_SUCCESS) {
    encode_msg_1 (MSG_ID_BOARD_READ_ERROR, SERIAL_ID_TO_IGNORE, status);
    mode = mode_error;
  } else {
    read_value = data;
    mode_advance = 1;
  }
  
}

static void read_byte (uint8_t reg)
{
  if (twi_read_byte (reg, read_completed) == 0)
    encode_msg_1 (MSG_ID_DEBUG_N, SERIAL_ID_TO_IGNORE, 222);
}

static uint8_t is_calibraion_ready ()
{
#define READY_MASK 0x86

  /*
    we need to check that WriteReq & ParaCh & StaCal bits are zeroes,
    1000 0110 - 1 for bits we need to check,
    negate read_value and compare AND for equal to mask
  */

  return (((~read_value) & READY_MASK) == READY_MASK) ? 1 : 0;
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

  uint8_t mode_old = mode;

  if ((mode < mode_last)
      || (mode != mode_error))
    ++mode;

  mode_advance = 0;

  encode_msg_1 (MSG_ID_DEBUG_B, SERIAL_ID_TO_IGNORE, mode_old);
  
  switch (mode_old) {
  case mode_error:
    /* hang here forever */
    break;
  case mode_reset:
    /* wait 2 micro-seconds after reset */
    /* _delay_ms (1); */
    mode_advance = 1;
    break;
  case mode_enable_start:
    /* read_byte (REG_CONTROL_1); */
    write_byte (REG_CONTROL_1, 0x80);
    break;
  case mode_enable_read: 
    /* encode_msg_1 (MSG_ID_DEBUG_M, SERIAL_ID_TO_IGNORE, read_value); */
    read_byte (REG_CONTROL_1);
    break;
  case mode_enable_check:
    if (is_calibraion_ready () == 0) {
      encode_msg_1 (MSG_ID_DEBUG_L, SERIAL_ID_TO_IGNORE, read_value);
      /* read_byte (REG_CONTROL_1); */
      mode -= 3;                /* write control-1 again */
      mode_advance = 1;
    }
    break;
  case mode_use_channel:
    write_byte (REG_USE_CHANNEL, 0xFF); /* all of them */
    break;
  case mode_gain:
    /* ! 8 registers */
    write_word (REG_GAIN, 0x60);
    break;
  case mode_threshold:
    /* ! 8 registers */
    write_word (REG_THRESHOLD, 0x32);
    break;
  case mode_average_count:
    write_byte (REG_AVERAGE_COUNT, 0x40);
    break;
  case mode_filter:
    write_byte (REG_FILTER, 0x04);
    break;
  case mode_debounce_count:
    write_byte (REG_DEBOUNCE_COUNT, 0x02);
    break;
  case mode_offcal_cycle:
    write_byte (REG_OFFCAL_CYCLE, 0x01);
    break;
  case mode_offcal_plus:
    write_byte (REG_OFFCAL_PLUS, 0x03);
    break;
  case mode_offcal_minus:
    write_byte (REG_OFFCAL_MINUS, 0x03);
    break;
  case mode_cancel_count_lower:
    write_byte (REG_CANCEL_COUNT_LOWER, 0x58);
    break;
  case mode_cancel_count_higher:
    write_byte (REG_CANCEL_COUNT_HIGHER, 0x02);
    break;
  case mode_cdac_base:
    write_byte (REG_CDAC_BASE, 0x80);
    break;
  case mode_measurument_mode:
    write_byte (REG_MEASUREMENT_MODE, 0x00);
    break;
  case mode_param_apply:
    write_byte (REG_CONTROL_1, 0x87);
    break;
  case mode_wakeup_write:
    write_byte (REG_CONTROL_2, 0x01);
    break;
  case mode_wakeup_wait:
    // waiting for interrupt here
    mode_advance = 1;
    break;
  case mode_read_error_status:
    read_byte (REG_ERROR_STATUS);
    break;
  case mode_check_error_status:
    if ((read_value & 0x81) != 0) {
      /* either syserr or calerr */
      mode = mode_error;
    }
    mode_advance = 1;
    break;
  case mode_read_data:
    /* try to read button state (pressed or released) */
    read_byte (REG_DATA);
    break;
  case mode_handle_data:
    handle_data (read_value);
    mode_advance = 1;
    break;
  /* case mode_data_read: */
  /*   read_byte (REG_DATA); */
  /*   break; */
  /* case mode_data_handle: */
  /*   mode_advance = 1; */
  /*   handle_data (read_value); */
  /*   break; */
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

void keyboard_enable ()
{
  mode = mode_reset;
  board_reset ();
}

void keyboard_disable ()
{
  mode = mode_error;
}

/* handle external interrupt from board */
ISR (INT2_vect)
{
  mode_advance = 1;
  encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, 111);
}
