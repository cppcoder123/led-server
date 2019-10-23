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
#define REG_CIN_0 0x11
/* #define REG_CIN_1 0x12 */
/* #define REG_CIN_2 0x13 */
/* #define REG_CIN_3 0x14 */
/* #define REG_CIN_4 0x15 */
/* #define REG_CIN_5 0x16 */
/* #define REG_CIN_6 0x17 */
/* #define REG_CIN_7 0x18 */
#define REG_ERROR 0x1A
#define REG_CHANNEL_ERROR 0x1B
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
  mode_enable_start,            /* start calibration */
  mode_enable_read,             /* read 'start' status */
  mode_enable_check,            /* check 'start' status */
  mode_use_channel,             /* param */
  mode_gain,                    /* param */
  mode_threshold,               /* param */
  mode_average_count,           /* param */
  mode_filter,                  /* param */
  mode_debounce_count,          /* param 10 */
  mode_offcal_cycle,            /* param */
  mode_offcal_plus,             /* param */
  mode_offcal_minus,            /* param */
  mode_cancel_count_lower,      /* param */
  mode_cancel_count_higher,     /* param */
  mode_cdac_base,               /* param */
  mode_measurument_mode,        /* param */
  mode_param_apply,             /* apply new params */
  mode_wakeup_write,            /* write wakeup command */
  mode_wakeup_write_ack,        /* handle write ack (do nothing) 20 */
  /* main loop */
  mode_wakeup_wait,             /* wait the interrupt 21*/
  mode_wakeup_wait_check,
  mode_read_channel_error,      /* error per channel */
  mode_check_channel_error,     /* print channel error */
  mode_read_error,              /* read error */
  mode_check_error,             /* check error */
  mode_read_cin,                /* read cin values */
  /* mode_read_cin_1, */
  /* mode_read_cin_2, */
  /* mode_read_cin_3, */
  /* mode_read_cin_4, */
  /* mode_read_cin_5, */
  /* mode_read_cin_6, */
  /* mode_read_cin_7, */
  /* mode_done_cin, */
  mode_dump_cin,
  mode_read_data,               /* read buttons states */
  mode_handle_data,             /* handle button data */
  mode_main_wakeup,             /* start mesurement again */
  mode_main_wakeup_ack,         /* write reg ack */
  mode_last = mode_main_wakeup_ack /* last mode */
};
static uint8_t mode = mode_reset;
static volatile uint8_t advance = 0;

/* state of our buttons */
static uint8_t state = 0;

/* read register value */
static uint8_t byte_buf = 0;
static uint8_t word_buf[TWI_WORD_SIZE];

static void advance_flag (uint8_t way)
{
  /* apply flag mask */
  advance |= way & ADVANCE_FLAG_MASK;
}

static void board_reset ()
{
  /* set portc 0th bit to 0, then to 1 */
  PORTC &= ~(1 << PORT_RESET);
  _delay_ms (1);
  PORTC |= (1 << PORT_RESET);

  mode = mode_reset;
  advance_flag (ADVANCE_FLAG_INT);
}

void keyboard_init ()
{
  mode = mode_error;            /* it should be enabled first */
  advance = 0;

  state = 0;

  byte_buf = 0;
  for (uint8_t i = 0; i < TWI_WORD_SIZE; ++i)
    word_buf[i] = 0;

  /* don't do it here to prevent enable before 'keyboard_enable' call */
  EICRA |= (1 << ISC20) | (1 << ISC21); /* rising edge */
  EIMSK |= (1 << INT2);                 /* enable int2 interrupt */

  /* PORT_RESET drives reset line, configure it as output, write 1 */
  DDRC |= (1 << PORT_RESET);
  PORTC |= (1 << PORT_RESET);
}

static void advance_go (uint8_t way)
{
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

static void advance_clear ()
{
  advance &= (ADVANCE_FLAG_MASK | ADVANCE_GO_MASK);
}

static void write_completed (uint8_t status)
{
  encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, 88);
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
  encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, 66);
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

static void read_word_completed (uint8_t status, volatile uint8_t *data)
{
  encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, 77);
  if (status != TWI_SUCCESS) {
    encode_msg_1 (MSG_ID_BOARD_READ_ERROR, SERIAL_ID_TO_IGNORE, status);
    mode = mode_error;
  } else {
    for (uint8_t i = 0; i < TWI_WORD_SIZE; ++i)
      word_buf[i] = data[i];
    advance_go (ADVANCE_FLAG_RW);
  }
}

static void read_word (uint8_t reg)
{
  if (twi_read_word (reg, read_word_completed) == 0) {
    encode_msg_1 (MSG_ID_BOARD_READ_ERROR,
                  SERIAL_ID_TO_IGNORE, TWI_READ_WORD_ERROR);
    twi_debug_cb ();
  }
}

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

void keyboard_try ()
{
  if (advance_check () == 0)
    return;

  uint8_t mode_old = mode;

  if ((mode < mode_last)
      && (mode != mode_error))
    ++mode;

  advance_clear ();
  
  /* in most cases we are waiting for rw */
  advance_flag (ADVANCE_FLAG_RW);

  encode_msg_2 (MSG_ID_DEBUG_B, SERIAL_ID_TO_IGNORE, mode_old, 109);
  
  switch (mode_old) {
  case mode_error:
    /* hang here forever */
    break;
  case mode_reset:
    /* wait 2 micro-seconds after reset */
    /* _delay_ms (1); */
    advance_go (ADVANCE_FLAG_RW);
    break;
  case mode_enable_start:
    /* read_byte (REG_CONTROL_1); */
    advance_flag (ADVANCE_FLAG_INT);
    write_byte (REG_CONTROL_1, 0x80);
    break;
  case mode_enable_read: 
    /* encode_msg_1 (MSG_ID_DEBUG_M, SERIAL_ID_TO_IGNORE, byte_buf); */
    read_byte (REG_CONTROL_1);
    /* read_byte (REG_USE_CHANNEL); */
    break;
  case mode_enable_check:
    if (is_ready () == 0) {
      encode_msg_1 (MSG_ID_DEBUG_L, SERIAL_ID_TO_IGNORE, byte_buf);
      mode -= 2;                /* read control-1 again */
    }
    advance_go (ADVANCE_FLAG_RW);
    break;
  case mode_use_channel:
    /* all of them FF */
    write_byte (REG_USE_CHANNEL, 0x0F);
    /* write_byte (REG_USE_CHANNEL, 0x1F); */
    /* write_byte (REG_USE_CHANNEL, 0x0F); */
    break;
  case mode_gain:
    /* ! 8 registers */
    /* write_word (REG_GAIN, 0x60); */
    /* write_word (REG_GAIN, 0xD0); */
    write_word (REG_GAIN, 0x60);
    break;
  case mode_threshold:
    /* ! 8 registers */
    /* write_word (REG_THRESHOLD, 0x32); */
    write_word (REG_THRESHOLD, 0x31);
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
    /* write_byte (REG_OFFCAL_PLUS, 0x03); */
    write_byte (REG_OFFCAL_PLUS, 0x01);
    break;
  case mode_offcal_minus:
    /* write_byte (REG_OFFCAL_MINUS, 0x03); */
    write_byte (REG_OFFCAL_MINUS, 0x01);
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
    advance_flag (ADVANCE_FLAG_INT); /* wait rw & int 2 advance steps*/
    write_byte (REG_CONTROL_2, 0x01);
    break;
  case mode_wakeup_write_ack:
    /* next step is to wait hw interrupt */
    /* goto next state but wait for interrupt */
    break;
  case mode_wakeup_wait:
     /* waiting for interrupt here */
    /* mode_advance = 1; */
    advance_clear ();
    advance_flag (ADVANCE_FLAG_INT); /* wait int as well */
    read_byte (REG_ERROR);
    break;
  case mode_wakeup_wait_check:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, byte_buf); */
    break;
  case mode_read_channel_error:
    read_byte (REG_CHANNEL_ERROR);
    break;
  case mode_check_channel_error:
    if (byte_buf != 0)
      encode_msg_1 (MSG_ID_DEBUG_Y, SERIAL_ID_TO_IGNORE, byte_buf);
    advance_go (ADVANCE_FLAG_RW);
    break;
  case mode_read_error:
    read_byte (REG_ERROR);
    /* read_byte (REG_CHANNEL_ERROR); */
    break;
  case mode_check_error:
    /* encode_msg_1 (MSG_ID_DEBUG_Z, SERIAL_ID_TO_IGNORE, byte_buf); */
    if ((byte_buf & 0x81) != 0) {
      encode_msg_1 (MSG_ID_DEBUG_U, SERIAL_ID_TO_IGNORE, byte_buf);
      /* either syserr or calerr */
      mode = mode_error;
    }
    advance_go (ADVANCE_FLAG_RW);
    break;
  case mode_read_cin:
    read_word (REG_CIN_0);
    break;
  /* case mode_read_cin_1: */
  /*   /\* encode_msg_1 (MSG_ID_DEBUG_Y, SERIAL_ID_TO_IGNORE, byte_buf); *\/ */
  /*   read_byte (REG_CIN_1); */
  /*   break; */
  /* case mode_read_cin_2: */
  /*   /\* encode_msg_1 (MSG_ID_DEBUG_Y, SERIAL_ID_TO_IGNORE, byte_buf); *\/ */
  /*   read_byte (REG_CIN_2); */
  /*   break; */
  /* case mode_read_cin_3: */
  /*   /\* encode_msg_1 (MSG_ID_DEBUG_Y, SERIAL_ID_TO_IGNORE, byte_buf); *\/ */
  /*   read_byte (REG_CIN_3); */
  /*   break; */
  /* case mode_read_cin_4: */
  /*   /\* encode_msg_1 (MSG_ID_DEBUG_Y, SERIAL_ID_TO_IGNORE, byte_buf); *\/ */
  /*   read_byte (REG_CIN_4); */
  /*   break; */
  /* case mode_read_cin_5: */
  /*   /\* encode_msg_1 (MSG_ID_DEBUG_Y, SERIAL_ID_TO_IGNORE, byte_buf); *\/ */
  /*   read_byte (REG_CIN_5); */
  /*   break; */
  /* case mode_read_cin_6: */
  /*   /\* encode_msg_1 (MSG_ID_DEBUG_Y, SERIAL_ID_TO_IGNORE, byte_buf); *\/ */
  /*   read_byte (REG_CIN_6); */
  /*   break; */
  /* case mode_read_cin_7: */
  /*   /\* encode_msg_1 (MSG_ID_DEBUG_Y, SERIAL_ID_TO_IGNORE, byte_buf); *\/ */
  /*   read_byte (REG_CIN_7); */
  /*   break; */
  /* case mode_done_cin: */
  /*   /\* encode_msg_1 (MSG_ID_DEBUG_Y, SERIAL_ID_TO_IGNORE, byte_buf); *\/ */
  /*   mode_advance = 1; */
  /*   break; */
  case mode_dump_cin:
    encode_msg_8 (MSG_ID_DEBUG_S, SERIAL_ID_TO_IGNORE,
                  word_buf[0], word_buf[1], word_buf[2], word_buf[3],
                  word_buf[4], word_buf[5], word_buf[6], word_buf[7]);
    advance_go (ADVANCE_FLAG_RW);
    break;
  case mode_read_data:
    encode_msg_1 (MSG_ID_DEBUG_Y, SERIAL_ID_TO_IGNORE, 222);
    /* try to read button state (pressed or released) */
    read_byte (REG_DATA);
    break;
  case mode_handle_data:
    /* encode_msg_1 (MSG_ID_DEBUG_T, SERIAL_ID_TO_IGNORE, byte_buf); */
    handle_data (byte_buf);
    advance_go (ADVANCE_FLAG_RW);
    break;
  case mode_main_wakeup:
    write_byte (REG_CONTROL_2, 0x01);
    break;
  case mode_main_wakeup_ack:
    mode = mode_wakeup_wait;
    advance_go (ADVANCE_FLAG_RW);
    break;
  default:
    advance_go (ADVANCE_FLAG_RW);
    encode_msg_1 (MSG_ID_BOARD_HANDLE_ERROR,
                  SERIAL_ID_TO_IGNORE, mode_old);
    mode = mode_error;
    break;
  }
}

void keyboard_enable ()
{
  /* EICRA |= (1 << ISC20) | (1 << ISC21); */ /* rising edge */
  /* EIMSK |= (1 << INT2);  */                /* enable int2 interrupt */

  twi_init ();

  state = 0;

  board_reset ();
}

void keyboard_disable ()
{
  mode = mode_error;
}

/* handle external interrupt from board */
ISR (INT2_vect)
{
  /* undocumented reset in mode_enable_read ? */
  /*   after writing 0x80 to REG_CONTROL_1 */

  /* if (mode != mode_enable_read) */
  /*   advance = 1; */

  advance_go (ADVANCE_FLAG_INT);

  encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, 55);
}
