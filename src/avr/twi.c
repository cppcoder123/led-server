
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/twi.h>

#include "unix/constant.h"

#include "debug.h"
#include "twi.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define STATUS_MASK (TWSR & 0xF8)
/* #define CONTROL_MASK ((1 << TWEN) | (1 << TWIE)) */

#define SLAVE_ADDRESS 0x68

enum {
  MODE_WRITE_START,     /* W: start writing */
  MODE_WRITE_SLAVE,     /* W: slave device addr, w */
  MODE_WRITE_REG,       /* W: slave register addr */
  MODE_WRITE_VALUE,     /* W: slave register value */
  /* MODE_WRITE_STOP,  */     /* W: end of writing */
  MODE_READ_START,      /* R: start reading */
  MODE_READ_SLAVE,      /* R: slave addr, r */
  MODE_READ_VALUE,      /* R: slave read value */
  MODE_READ_DONE,       /* R: time to send STOP */
  /* MODE_READ_STOP,   */     /* R: stop reading */
  MODE_IDLE,
};
static volatile uint8_t mode = MODE_IDLE;

static volatile uint8_t status = TWI_SUCCESS;

static volatile uint8_t reg_buf = 0;
static volatile uint8_t data_buf = 0;

static twi_write_callback write_cb;
static twi_read_callback read_cb;

/* static volatile uint8_t transfer_count = 0; /\* in either direction *\/ */
/* static volatile uint8_t transfer_limit = TRANSFER_BYTE; */

void twi_init ()
{
  write_cb = NULL;
  read_cb = NULL;

  /* transfer_count = 0; */
  /* transfer_limit = TRANSFER_BYTE; */

  /* bit rate: scl rate should 4*10^6 / (16 + 2 * TWBR * (4 ^TWPS)) */
  /* false: so 4*10^6 / (16 + 2 * 0x08 * 1)  = aprox 102kHz */
  /* TWBR = 0x08; */
  /* TWBR = 0xFF; */
  /* TWBR = 0x08; */
  /* TWBR = 0x02; */
  TWBR = 0x04;
  /* TWSR |= (1 << TWPS0) | (1 << TWPS1);  */ /* min clock freq fixme: remove */

  /* enable twi interrupt */
  TWCR |= (1 << TWEN) | (1 << TWIE);
}

static uint8_t busy ()
{
  return ((write_cb != NULL) || (read_cb != NULL)) ? 1 : 0;
}

void twi_try ()
{
  if ((mode != MODE_IDLE)
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
  mode = (new_mode == MODE_WRITE_START) ? MODE_WRITE_START : MODE_READ_START;

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

  mode = MODE_IDLE;
}

uint8_t twi_read_byte (uint8_t reg, twi_read_callback cb)
{
  if (busy () != 0)
    return 0;

  status = TWI_SUCCESS;
  read_cb = cb;

  reg_buf = reg;
  data_buf = 111;

  start (MODE_WRITE_START);
  
  return 1;
}

uint8_t twi_write_byte (uint8_t reg, uint8_t value, twi_write_callback cb)
{
  if (busy () != 0)
    return 0;

  status = TWI_SUCCESS;
  write_cb = cb;

  reg_buf = reg;
  data_buf = value;

  start (MODE_WRITE_START);

  return 1;
}

void twi_debug_cb ()
{
  if (write_cb != NULL)
    debug_0 (DEBUG_TWI, 0);
  if (read_cb != NULL)
    debug_0 (DEBUG_TWI, 1);
  if ((write_cb == NULL) && (read_cb == NULL))
    debug_0 (DEBUG_TWI, 2);
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
  if (mode < MODE_IDLE)
    ++mode;

  /* if (mode != MODE_IDLE) */
  /* encode_msg_1 (MSG_ID_DEBUG_H, SERIAL_ID_TO_IGNORE, mode_old); */

  switch (mode_old) {
  case MODE_WRITE_START:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 70); */
    if (check_status_register (TW_START) == 0) {
      debug_1 (DEBUG_TWI, 3, STATUS_MASK);
      status = TWI_WRITE_START_ERROR;
      stop ();
    } else {
      TWDR = slave (0);         /* slave addr, writing */
      /* TWCR |= (1 << TWEN); */
      TWCR &= ~(1 << TWSTA);
    }
    break;
  case MODE_WRITE_SLAVE:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 80); */
    if (check_status_register (TW_MT_SLA_ACK) == 0) {
      debug_1 (DEBUG_TWI, 4, STATUS_MASK);
      status = TWI_WRITE_SLAVE_ERROR;
      stop ();
    } else {
      /* encode_msg_1 (MSG_ID_DEBUG_G, SERIAL_ID_TO_IGNORE, buf[0]); */
      /* TWCR &= ~(1 << TWSTA); */
      TWDR = reg_buf;
    }
    break;
  case MODE_WRITE_REG:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90); */
    if (check_status_register (TW_MT_DATA_ACK) == 0) {
      /* encode_msg_2 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90, 1); */
      debug_1 (DEBUG_TWI, 5, STATUS_MASK);
      /* either reg error or value error */
      status = TWI_WRITE_REG_ERROR;
      stop ();
    } else {
      if (reading () != 0) {
        /* encode_msg_2 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90, 2); */
        start (MODE_READ_START);
      } else {
        /* encode_msg_2 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90, 3); */
        TWDR = data_buf;
      }
    }
    break;
  case MODE_WRITE_VALUE:
    /* encode_msg_1 (MSG_ID_DEBUG_O, SERIAL_ID_TO_IGNORE, STATUS_MASK); */
    if (check_status_register (TW_MT_DATA_ACK) == 0) {
      /* encode_msg_1 (MSG_ID_DEBUG_O, SERIAL_ID_TO_IGNORE, 11); */
      status = TWI_WRITE_VALUE_ERROR;
      stop ();
    } /* else if (transfer_count < transfer_limit) { */
    /*   /\* encode_msg_1 (MSG_ID_DEBUG_O, SERIAL_ID_TO_IGNORE, 22); *\/ */
    /*   ++transfer_count; */
    /*   mode -= 2;              /\* go to write reg again *\/ */
    /* }  */
    else {
      /* encode_msg_1 (MSG_ID_DEBUG_O, SERIAL_ID_TO_IGNORE, 33); */
      stop ();
    }
    break;
  /* case MODE_WRITE_STOP: */
  /*   mode = MODE_IDLE; */
  /*   fixme; */
  /*   break; */
  case MODE_READ_START:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 100); */
    if (check_status_register (TW_REP_START) == 0) {
      status = TWI_READ_START_ERROR;
      stop ();
    } else {
      /* TWCR &= ~(1 << TWSTA); */
      TWDR = slave (1);         /* slave addr, reading */
    }
    break;
  case MODE_READ_SLAVE:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 110); */
    if (check_status_register (TW_MR_SLA_ACK) == 0) {
      debug_1 (DEBUG_TWI, 6, STATUS_MASK);
      status = TWI_READ_SLAVE_ERROR;
      stop ();
    } else {
       TWCR &= ~(1 << TWSTA);
       /* if (transfer_count < transfer_limit) */
       /*   TWCR |= (1 << TWEA); */
    }
    break;
  case MODE_READ_VALUE:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 120); */
    if (check_status_register (TW_MR_DATA_ACK) == 0) {
      debug_1 (DEBUG_TWI, 7, STATUS_MASK);
      status = TWI_READ_VALUE_ERROR;
      stop ();
    } else {
      /* data_buf[transfer_count] = TWDR; */
      data_buf = TWDR;
      /* if (transfer_count < transfer_limit) { */
      /*   /\* encode_msg_1 (MSG_ID_DEBUG_Z, SERIAL_ID_TO_IGNORE, data_buf[transfer_count]); *\/ */
      /*   ++transfer_count; */
      /*   --mode;                 /\* the same mode *\/ */
      /*   TWCR |= (1 << TWEA); */
      /* } else { */
        TWCR &= ~(1 << TWEA);   /* send nack - do we need this ? */
        /* stop (); */
      /* } */
    }
    break;
  case MODE_READ_DONE:
    if (check_status_register (TW_MR_DATA_NACK) == 0) {
      debug_1 (DEBUG_TWI, 8, STATUS_MASK);
      status = TWI_READ_DONE_ERROR;
    }
    stop ();
    break;
  case MODE_IDLE:
    /* is it possible to get here? not sure */
    mode = MODE_IDLE;
    break;
  default:
    break;
  }

  /* if (mode != MODE_IDLE) */
  /*   TWCR |= CONTROL_MASK; */
  /* interrupt is handled by writing 1 */
  TWCR |= (1 << TWINT);
}
