
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/twi.h>

#include "mcu/constant.h"

#include "encode.h"
#include "twi.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define STATUS_MASK (TWSR & 0xF8)
/* #define CONTROL_MASK ((1 << TWEN) | (1 << TWIE)) */

#define SLAVE_ADDRESS 0x17      /* addr selection wire to +5v */

#define TRANSFER_BYTE 0
#define TRANSFER_WORD 7

enum {
  mode_write_start,     /* W: start writing */
  mode_write_slave,     /* W: slave device addr, w */
  mode_write_reg,       /* W: LC717 register addr */
  mode_write_value,     /* W: LC717 register value */
  /* mode_write_stop,  */     /* W: end of writing */
  mode_read_start,      /* R: start reading */
  mode_read_slave,      /* R: slave addr, r */
  mode_read_value,      /* R: slave read value */
  mode_read_done,       /* R: time to send STOP */
  /* mode_read_stop,   */     /* R: stop reading */
  mode_idle,
  mode_last = mode_idle
};
static volatile uint8_t mode = mode_idle;

static volatile uint8_t status = TWI_SUCCESS;

static volatile uint8_t reg_buf = 0;
static volatile uint8_t data_buf[TWI_WORD_SIZE];

static twi_write_callback write_cb;
static twi_read_callback read_cb;

static volatile uint8_t transfer_count = 0; /* in either direction */
static volatile uint8_t transfer_limit = TRANSFER_BYTE;

void twi_init ()
{
  write_cb = NULL;
  read_cb = NULL;

  transfer_count = 0;
  transfer_limit = TRANSFER_BYTE;

  /* bit rate: scl rate should 4*10^6 / (16 + 2 * TWBR * (4 ^TWPS)) */
  /* false: so 4*10^6 / (16 + 2 * 0x08 * 1)  = aprox 102kHz */
  /* TWBR = 0x08; */
  /* TWBR = 0xFF; */
  /* TWBR = 0x08; */
  /* TWBR = 0x02; */
  TWBR = 0x04;
  /* TWSR |= (1 << TWPS0) | (1 << TWPS1);  min clock freq fixme: remove */

  /* enable twi interrupt */
  TWCR |= (1 << TWEN) | (1 << TWIE);
}

static uint8_t busy ()
{
  return ((write_cb != NULL) || (read_cb != NULL)) ? 1 : 0;
}

void twi_try ()
{
  if ((mode != mode_idle)
      || (busy () == 0))
    return;

  if (write_cb) {
    write_cb (status);
    write_cb = NULL;
  } else if (read_cb) {
    read_cb (status, data_buf);
    read_cb = NULL;
  }
}

static void start (uint8_t new_mode)
{
  mode = (new_mode == mode_write_start) ? mode_write_start : mode_read_start;

  TWCR |= (1 << TWSTA);
  TWCR &= ~(1 << TWSTO);
  
  /* TWCR |= (1 << TWEN) | (1 << TWIE) | (1 << TWEA); */
  TWCR |= (1 << TWEN) | (1 << TWIE);
}

static void stop ()
{
  TWCR |= (1 << TWSTO);
  TWCR &= ~(1 << TWSTA);

  /* TWCR &= ~(1 << TWEN); */

  mode = mode_idle;
}

static uint8_t twi_read (uint8_t reg, twi_read_callback cb)
{
  if (busy () != 0)
    return 0;

  status = TWI_SUCCESS;
  read_cb = cb;
  reg_buf = reg;

  transfer_count = 0;

  start (mode_write_start);
  
  return 1;
}

uint8_t twi_read_byte (uint8_t reg, twi_read_callback cb)
{
  transfer_limit = TRANSFER_BYTE;

  data_buf[0] = 111;

  return twi_read (reg, cb);
}

uint8_t twi_read_word (uint8_t reg, twi_read_callback cb)
{
  transfer_limit = TRANSFER_WORD;

  return twi_read (reg, cb);
}

static uint8_t twi_write (uint8_t reg, uint8_t value, twi_write_callback cb)
{
  if (busy () != 0)
    return 0;

  status = TWI_SUCCESS;
  write_cb = cb;
  reg_buf = reg;

  /* we don't need to write different values for word bytes now */
  data_buf[0] = value;

  transfer_count = 0;

  start (mode_write_start);

  return 1;
}

uint8_t twi_write_byte (uint8_t reg, uint8_t value, twi_write_callback cb)
{
  transfer_limit = TRANSFER_BYTE;

  return twi_write (reg, value, cb);
}

uint8_t twi_write_word (uint8_t reg, uint8_t value, twi_write_callback cb)
{
  transfer_limit = TRANSFER_WORD;

  return twi_write (reg, value, cb);
}

void twi_debug_cb ()
{
  if (write_cb != NULL)
    encode_msg_1 (MSG_ID_DEBUG_R, SERIAL_ID_TO_IGNORE, 100);
  if (read_cb != NULL)
    encode_msg_1 (MSG_ID_DEBUG_R, SERIAL_ID_TO_IGNORE, 200);
  if ((write_cb == NULL) && (read_cb == NULL))
    encode_msg_1 (MSG_ID_DEBUG_R, SERIAL_ID_TO_IGNORE, 0);
}

static uint8_t reading ()
{
  return (read_cb != 0) ? 1 : 0;
}

static uint8_t slave (uint8_t read)
{
  uint8_t addr = (SLAVE_ADDRESS << 1) | ((read != 0) ? 1 : 0);
  /* encode_msg_1 (MSG_ID_DEBUG_C, SERIAL_ID_TO_IGNORE, addr); */
  return addr;
}

static uint8_t check_status_register (uint8_t mask)
{
  return ((STATUS_MASK & mask) == mask) ? 1 : 0;
}

ISR (TWI_vect)
{
  /* TWCR |= (1 << TWEN) | (1 << TWIE); */
  
  uint8_t mode_old = mode;
  if (mode < mode_idle)
    ++mode;

  /* if (mode != mode_idle) */
  /* encode_msg_1 (MSG_ID_DEBUG_H, SERIAL_ID_TO_IGNORE, mode_old); */

  switch (mode_old) {
  case mode_write_start:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 70); */
    if (check_status_register (TW_START) == 0) {
      encode_msg_1 (MSG_ID_DEBUG_J, SERIAL_ID_TO_IGNORE, STATUS_MASK);
      status = TWI_WRITE_START_ERROR;
      stop ();
    } else {
      TWDR = slave (0);         /* slave addr, writing */
      /* TWCR |= (1 << TWEN); */
      /* TWCR &= ~(1 << TWSTA); */
    }
    break;
  case mode_write_slave:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 80); */
    if (check_status_register (TW_MT_SLA_ACK) == 0) {
      encode_msg_1 (MSG_ID_DEBUG_D, SERIAL_ID_TO_IGNORE, STATUS_MASK);
      status = TWI_WRITE_SLAVE_ERROR;
      stop ();
    } else {
      /* encode_msg_1 (MSG_ID_DEBUG_G, SERIAL_ID_TO_IGNORE, buf[0]); */
      TWCR &= ~(1 << TWSTA);
      TWDR = reg_buf;
    }
    break;
  case mode_write_reg:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90); */
    if (check_status_register (TW_MT_DATA_ACK) == 0) {
      /* encode_msg_2 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90, 1); */
      encode_msg_1 (MSG_ID_DEBUG_E, SERIAL_ID_TO_IGNORE, STATUS_MASK);
      /* either reg error or value error */
      status = TWI_WRITE_REG_ERROR;
      stop ();
    } else {
      if (reading () != 0) {
        /* encode_msg_2 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90, 2); */
        start (mode_read_start);
      } else {
        /* encode_msg_2 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90, 3); */
        TWDR = data_buf[0];
      }
    }
    break;
  case mode_write_value:
    /* encode_msg_1 (MSG_ID_DEBUG_O, SERIAL_ID_TO_IGNORE, STATUS_MASK); */
    if (check_status_register (TW_MT_DATA_ACK) == 0) {
      /* encode_msg_1 (MSG_ID_DEBUG_O, SERIAL_ID_TO_IGNORE, 11); */
      status = TWI_WRITE_VALUE_ERROR;
      stop ();
    } else if (transfer_count < transfer_limit) {
      /* encode_msg_1 (MSG_ID_DEBUG_O, SERIAL_ID_TO_IGNORE, 22); */
      ++transfer_count;
      mode -= 2;              /* go to write reg again */
    } else {
      /* encode_msg_1 (MSG_ID_DEBUG_O, SERIAL_ID_TO_IGNORE, 33); */
      stop ();
    }
    break;
  /* case mode_write_stop: */
  /*   mode = mode_idle; */
  /*   fixme; */
  /*   break; */
  case mode_read_start:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 100); */
    if (check_status_register (TW_REP_START) == 0) {
      status = TWI_READ_START_ERROR;
      stop ();
    } else {
      /* TWCR &= ~(1 << TWSTA); */
      TWDR = slave (1);         /* slave addr, reading */
    }
    break;
  case mode_read_slave:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 110); */
    if (check_status_register (TW_MR_SLA_ACK) == 0) {
      encode_msg_1 (MSG_ID_DEBUG_K, SERIAL_ID_TO_IGNORE, STATUS_MASK);
      status = TWI_READ_SLAVE_ERROR;
      stop ();
    } else {
       TWCR &= ~(1 << TWSTA);
       if (transfer_count < transfer_limit)
         TWCR |= (1 << TWEA);
    }
    break;
  case mode_read_value:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 120); */
    if (check_status_register (TW_MR_DATA_ACK) == 0) {
      encode_msg_1 (MSG_ID_DEBUG_Q, SERIAL_ID_TO_IGNORE, STATUS_MASK);
      status = TWI_READ_VALUE_ERROR;
      stop ();
    } else {
      data_buf[transfer_count] = TWDR;
      if (transfer_count < transfer_limit) {
        /* encode_msg_1 (MSG_ID_DEBUG_Z, SERIAL_ID_TO_IGNORE, data_buf[transfer_count]); */
        ++transfer_count;
        --mode;                 /* the same mode */
        TWCR |= (1 << TWEA);
      } else {
        TWCR &= ~(1 << TWEA);   /* send nack - do we need this ? */
        /* stop (); */
      }
    }
    break;
  case mode_read_done:
    if (check_status_register (TW_MR_DATA_NACK) == 0) {
      encode_msg_1 (MSG_ID_DEBUG_P, SERIAL_ID_TO_IGNORE, STATUS_MASK);
      status = TWI_READ_DONE_ERROR;
    }
    stop ();
    break;
  case mode_idle:
    /* is it possible to get here? not sure */
    mode = mode_idle;
    break;
  default:
    break;
  }

  /* if (mode != mode_idle) */
  /*   TWCR |= CONTROL_MASK; */
  /* interrupt is handled by writing 1 */
  TWCR |= (1 << TWINT);
}
