/*
 *
 */

#define F_CPU 4000000UL

#include <avr/interrupt.h>
#include <stdint.h>
/* #include <util/atomic.h> */
#include <util/delay.h>

#include "mcu/constant.h"

#include "debug.h"
#include "encode.h"
#include "key-board.h"
#include "twi.h"

/* registers we want to access */
#define REG_CONTROL_1 0x1C
#define REG_CONTROL_2 0x40
#define REG_USE_CHANNEL 0x00
#define REG_GAIN 0x01           /* first gain register, 1 per channel */
#define REG_THRESHOLD 0x09      /* first threshold register, 1 per channel */
#define REG_CIN_0 0x11
#define REG_ERROR 0x1A
#define REG_CHANNEL_ERROR 0x1B
#define REG_AVERAGE_COUNT 0x1D
#define REG_FILTER 0x1E
#define REG_DEBOUNCE_COUNT 0x1F
#define REG_SHORT_INTERVAL 0x20
#define REG_LONG_INTERVAL 0x21
#define REG_OFFCAL_CYCLE 0x22
#define REG_OFFCAL_PLUS 0x23
#define REG_OFFCAL_MINUS 0x24
#define REG_CANCEL_COUNT_LOWER 0x25
#define REG_CANCEL_COUNT_HIGHER 0x26
#define REG_CDAC_BASE 0x3D
#define REG_MEASUREMENT_MODE 0x3E

#define REG_DATA 0x19  /* needed data should be here :) */

/* This is the way how we advance our state machine */
/* wait advance due to rw */
#define ADVANCE_FLAG_RW (1 << 0)
/* wait advance due to interrupt */
#define ADVANCE_FLAG_INT (1 << 1)
/* rw happened */
#define ADVANCE_GO_RW (1 << 2)
/* interrupt happened */
#define ADVANCE_GO_INT (1 << 3)

#define ADVANCE_FLAG_MASK (ADVANCE_FLAG_RW | ADVANCE_FLAG_INT)
#define ADVANCE_GO_MASK (ADVANCE_GO_RW | ADVANCE_GO_INT)
#define ADVANCE_RW_MASK (ADVANCE_FLAG_RW | ADVANCE_GO_RW)
#define ADVANCE_INT_MASK (ADVANCE_FLAG_INT | ADVANCE_GO_INT)

/* lc717 reset line driver */
#define PORT_RESET PORTC0

/* mode of operation */
enum {
  mode_error,                   /* smth bad happened */
  /* setup parameters */
  mode_reset,                   /* reset */
  /* mode_enable_start, */            /* start calibration */
  mode_enable_read,             /* read 'start' status */
  mode_enable_check,            /* check 'start' status */
  mode_use_channel,             /* param */
  mode_gain,                    /* param */
  mode_threshold,               /* param */
  mode_average_count,           /* param */
  mode_filter,                  /* param */
  mode_debounce_count,          /* param 10 */
  mode_short_interval,           /* param 0x05 */
  mode_long_interval,            /* param 0x01 */
  mode_offcal_cycle,            /* param */
  mode_offcal_plus,             /* param */
  mode_offcal_minus,            /* param */
  mode_cancel_count_lower,      /* param */
  mode_cancel_count_higher,     /* param */
  mode_cdac_base,               /* param */
  mode_measurument_mode,        /* param */
  mode_param_apply,             /* apply new params */
  mode_wakeup_wait,             /* wait the interrupt 20*/
  /* main loop */
  mode_loop_start,              /* main loop start */
  mode_read_error,              /* read error */
  mode_check_error,             /* check error */
  mode_read_data,               /* read buttons states */
  mode_handle_data,             /* handle button data */
  mode_loop_finish,
};
volatile uint8_t mode = mode_reset;
volatile uint8_t advance = 0;

/* state of our buttons */
static uint8_t state = 0;

/* read register value */
static uint8_t byte_buf = 0;
static uint8_t word_buf[TWI_WORD_SIZE];


/* static void advance_clear () */
/* { */
/*   advance &= ~(ADVANCE_FLAG_MASK | ADVANCE_GO_MASK); */
/* } */

static void advance_flag (uint8_t way)
{
  advance = 0;

  if ((way != ADVANCE_FLAG_RW) && (way != ADVANCE_FLAG_INT))
    return;

  advance = way;
}

static uint8_t advance_flag_empty ()
{
  return ((advance | ADVANCE_FLAG_MASK) == 0) ? 1 : 0;
}

static void board_reset ()
{
  /* set portc 0th bit to 0, then to 1 */
  PORTC &= ~(1 << PORT_RESET);
  _delay_ms (5);
  PORTC |= (1 << PORT_RESET);

  mode = mode_reset;
  advance_flag (ADVANCE_FLAG_INT);
}

void key_board_init ()
{
  mode = mode_error;            /* it should be enabled first */
  advance = 0;

  state = 0;

  byte_buf = 0;
  for (uint8_t i = 0; i < TWI_WORD_SIZE; ++i)
    word_buf[i] = 0;

  /* don't do it here to prevent enable before 'key_board_enable' call */
  /* EICRA |= (1 << ISC20) | (1 << ISC21); /\* rising edge *\/ */
  /* EIMSK |= (1 << INT2);                 /\* enable int2 interrupt *\/ */

  /* PORT_RESET drives reset line, configure it as output, write 1 */
  DDRC |= (1 << PORT_RESET);
  PORTC |= (1 << PORT_RESET);
}

static void advance_go (uint8_t way)
{
  if (advance & ADVANCE_GO_MASK)
    // prev state is not handled
    return;

  if (way & ADVANCE_FLAG_RW)
    advance |= ADVANCE_GO_RW;
  if (way & ADVANCE_FLAG_INT)
    advance |= ADVANCE_GO_INT;
}

static uint8_t advance_check ()
{
  /* nothing happened */
  if ((advance & ADVANCE_GO_MASK) == 0)
    return 0;

  if ((advance & ADVANCE_FLAG_MASK) == ADVANCE_FLAG_MASK)
    /* both actions should occur */
    return ((advance & ADVANCE_GO_MASK) == ADVANCE_GO_MASK) ? 1 : 0;
  
  if ((advance & ADVANCE_RW_MASK) == ADVANCE_RW_MASK)
    return 1;

  if ((advance & ADVANCE_INT_MASK) == ADVANCE_INT_MASK)
    return 1;

  return 0;
}

static void write_completed (uint8_t status)
{
  if (status != TWI_SUCCESS) {
    encode_msg_1 (MSG_ID_BOARD_WRITE_ERROR, SERIAL_ID_TO_IGNORE, status);
    mode = mode_error;
  } else {
    advance_go (ADVANCE_FLAG_RW);
  }
}

static void write_byte (uint8_t reg, uint8_t data)
{
  if (twi_write_byte (reg, data, write_completed) == 0) {
    encode_msg_1 (MSG_ID_BOARD_WRITE_ERROR,
                  SERIAL_ID_TO_IGNORE, TWI_WRITE_BYTE_ERROR);
    twi_debug_cb ();
  }
}

static void write_word (uint8_t reg, uint8_t data)
{
  if (twi_write_word (reg, data, write_completed) == 0) {
    encode_msg_1 (MSG_ID_BOARD_WRITE_ERROR,
                  SERIAL_ID_TO_IGNORE, TWI_WRITE_WORD_ERROR);
    twi_debug_cb ();
  }
}

static void read_byte_completed (uint8_t status, volatile uint8_t *data)
{
  if (status != TWI_SUCCESS) {
    encode_msg_1 (MSG_ID_BOARD_READ_ERROR, SERIAL_ID_TO_IGNORE, status);
    mode = mode_error;
  } else {
    byte_buf = *data;
    advance_go (ADVANCE_FLAG_RW);
  }
  
}

static void read_byte (uint8_t reg)
{
  if (twi_read_byte (reg, read_byte_completed) == 0) {
    encode_msg_1 (MSG_ID_BOARD_READ_ERROR,
                  SERIAL_ID_TO_IGNORE, TWI_READ_BYTE_ERROR);
    twi_debug_cb ();
  }
}

/* static void read_word_completed (uint8_t status, volatile uint8_t *data) */
/* { */
/*   /\* encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, 77); *\/ */
/*   if (status != TWI_SUCCESS) { */
/*     encode_msg_1 (MSG_ID_BOARD_READ_ERROR, SERIAL_ID_TO_IGNORE, status); */
/*     mode = mode_error; */
/*   } else { */
/*     for (uint8_t i = 0; i < TWI_WORD_SIZE; ++i) */
/*       word_buf[i] = data[i]; */
/*     advance_go (ADVANCE_FLAG_RW); */
/*   } */
/* } */

/* static void read_word (uint8_t reg) */
/* { */
/*   if (twi_read_word (reg, read_word_completed) == 0) { */
/*     encode_msg_1 (MSG_ID_BOARD_READ_ERROR, */
/*                   SERIAL_ID_TO_IGNORE, TWI_READ_WORD_ERROR); */
/*     twi_debug_cb (); */
/*   } */
/* } */

static uint8_t is_ready ()      /* to go ahead */
{
#define READY_MASK 0x86

  /*
    we need to check that WriteReq & ParaCh & StaCal bits are zeroes,
    1000 0110 - 1 for bits we need to check,
    negate byte_buf and compare AND for equal to mask
  */

  return (((~byte_buf) & READY_MASK) == READY_MASK) ? 1 : 0;
}

static void handle_data (uint8_t new_state)
{
  if (new_state == state)
    return;

  encode_msg_2 (MSG_ID_BUTTON, SERIAL_ID_TO_IGNORE, new_state, state);

  state = new_state;
}

void key_board_try ()
{
  if ((mode == mode_error)
      || (advance_check () == 0))
    return;

  advance_flag (ADVANCE_FLAG_RW);
  int jump = 0;

  debug_1 (DEBUG_KEY_BOARD, DEBUG_15, mode);
  
  switch (mode) {
  case mode_error:
    /* hang here forever */
    break;
  case mode_reset:
    /* wait 1 micro-second after reset */
    /* _delay_ms (1); */
    write_byte (REG_CONTROL_1, 0x88);
    /* debug_0 (DEBUG_KEY_BOARD, DEBUG_9);     */
    /* advance_go (ADVANCE_FLAG_RW); */
    break;
  /* case mode_enable_start: */
  /*   debug_0 (DEBUG_KEY_BOARD, DEBUG_10);     */
  /*   break; */
  case mode_enable_read:
    /* debug_0 (DEBUG_KEY_BOARD, DEBUG_11); */
    /* byte_buf = 222; */
    read_byte (REG_CONTROL_1);
    break;
  case mode_enable_check:
    if (is_ready () == 0) {
      _delay_ms (5);
      debug_1 (DEBUG_KEY_BOARD, DEBUG_0, byte_buf);
      mode -= 2;                /* read control-1 again */
    }
    jump = 1;
    break;
  case mode_use_channel:
    /* all of them FF */
    write_byte (REG_USE_CHANNEL, 0xFF);
    break;
  case mode_gain:
    write_word (REG_GAIN, 0x50); /* recommended value */
    /* write_word (REG_GAIN, 0x20); */
    break;
  case mode_threshold:
    /* ! 8 registers */
    /* write_word (REG_THRESHOLD, 0x0A); recommended value */
    write_word (REG_THRESHOLD, 0x0A);
    break;
  case mode_average_count:
    /* write_byte (REG_AVERAGE_COUNT, 0x40); */
    write_byte (REG_AVERAGE_COUNT, 0x80);
    break;
  case mode_filter:
    /* write_byte (REG_FILTER, 0x04); 0x00 or 0x0C */
    write_byte (REG_FILTER, 0x0);
    break;
  case mode_debounce_count:
    /* write_byte (REG_DEBOUNCE_COUNT, 0x02); */
    write_byte (REG_DEBOUNCE_COUNT, 0x01);
    break;
  case mode_short_interval:
    write_byte (REG_SHORT_INTERVAL, 0x05); /*recommended value*/
    /* write_byte (REG_SHORT_INTERVAL, 0xFF); */
    break;
  case mode_long_interval:
    write_byte (REG_LONG_INTERVAL, 0x01); /*recommended value*/
    /* write_byte (REG_LONG_INTERVAL, 0xFF); */
    break;
  case mode_offcal_cycle:
    /* write_byte (REG_OFFCAL_CYCLE, 0x01); */
    write_byte (REG_OFFCAL_CYCLE, 0x04);
    break;
  case mode_offcal_plus:
    write_byte (REG_OFFCAL_PLUS, 0x03);
    /* write_byte (REG_OFFCAL_PLUS, 0x01); */
    break;
  case mode_offcal_minus:
    write_byte (REG_OFFCAL_MINUS, 0x03);
    /* write_byte (REG_OFFCAL_MINUS, 0x01); */
    break;
  case mode_cancel_count_lower:
    /* write_byte (REG_CANCEL_COUNT_LOWER, 0x58); */
    write_byte (REG_CANCEL_COUNT_LOWER, 0x32);
    break;
  case mode_cancel_count_higher:
    /* write_byte (REG_CANCEL_COUNT_HIGHER, 0x02); */
    write_byte (REG_CANCEL_COUNT_HIGHER, 0x01);
    break;
  case mode_cdac_base:
    write_byte (REG_CDAC_BASE, 0x80);
    break;
  case mode_measurument_mode:
    /* write_byte (REG_MEASUREMENT_MODE, 0x00); */
    write_byte (REG_MEASUREMENT_MODE, 0x00);
    break;
  case mode_param_apply:
    write_byte (REG_CONTROL_1, 0x8F);
    break;
  case mode_wakeup_wait:
    advance_flag (ADVANCE_FLAG_INT);
    /* _delay_ms (5); */
    break;
  case mode_loop_start:
    /* debug_0 (DEBUG_KEY_BOARD, DEBUG_2); */
    /* write_byte (REG_CONTROL_2, 0x00); */
    /* advance_flag (ADVANCE_FLAG_RW); */
    /* advance_go (ADVANCE_FLAG_RW); */
    jump = 1;
    break;
  case mode_read_error:
    /* debug_0 (DEBUG_KEY_BOARD, DEBUG_8); */
    read_byte (REG_ERROR);
    /* read_byte (REG_CHANNEL_ERROR); */
    break;
  case mode_check_error:
    /* debug_0 (DEBUG_KEY_BOARD, DEBUG_7); */
    if ((byte_buf & 0x81) != 0) {
      debug_1 (DEBUG_KEY_BOARD, DEBUG_4, byte_buf);
      /* either syserr or calerr */
      mode = mode_error;
    }
    /* advance_flag (ADVANCE_FLAG_RW); */
    /* advance_go (ADVANCE_FLAG_RW); */
    jump = 1;
    break;
  case mode_read_data:
    /* debug_0 (DEBUG_KEY_BOARD, DEBUG_6); */
    /* try to read button state (pressed or released) */
    read_byte (REG_DATA);
    break;
  case mode_handle_data:
    /* debug_0 (DEBUG_KEY_BOARD, DEBUG_5); */
    handle_data (byte_buf);
    /* advance_go (ADVANCE_FLAG_RW); */
    write_byte (REG_CONTROL_2, 0x00);
    break;
  case mode_loop_finish:
    /* debug_0 (DEBUG_KEY_BOARD, DEBUG_4); */
    /* mode = mode_loop_start; */
    advance_flag (ADVANCE_FLAG_INT);
    /* advance_go (ADVANCE_FLAG_RW); */
    break;
  default:
    debug_0 (DEBUG_KEY_BOARD, DEBUG_3);
    /* advance_flag (ADVANCE_FLAG_RW); */
    /* advance_go (ADVANCE_FLAG_RW); */
    jump = 1;
    encode_msg_1 (MSG_ID_BOARD_HANDLE_ERROR,
                  SERIAL_ID_TO_IGNORE, mode);
    mode = mode_error;
    break;
  }
  /* debug_1 (DEBUG_KEY_BOARD, DEBUG_20, mode); */

  /* if (advance_flag_empty () != 0) */
    /* in most cases we are waiting for rw */
    /* advance_flag (ADVANCE_FLAG_RW); */

  if (mode != mode_error) {
    if (mode < mode_loop_finish) {
      ++mode;
    } else {
      mode = mode_loop_start;
    }
  }

  if (jump != 0)
    advance_go (ADVANCE_FLAG_RW);

}

void key_board_enable ()
{
  EICRA |= (1 << ISC20) | (1 << ISC21); /* rising edge */
  EIMSK |= (1 << INT2);                 /* enable int2 interrupt */

  twi_init ();

  state = 0;

  board_reset ();
}

void key_board_disable ()
{
  EICRA &= ~((1 << ISC20) | (1 << ISC21)); /* rising edge */
  EIMSK &= ~((1 << INT2));                 /* enable int2 interrupt */

  mode = mode_error;
}

/* handle external interrupt from board */
ISR (INT2_vect)
{
  advance_go (ADVANCE_FLAG_INT);

  debug_1 (DEBUG_KEY_BOARD, DEBUG_16, mode);
}
