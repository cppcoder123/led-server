/*
 *
 */
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/twi.h>

#include "unix/constant.h"

#include "buf.h"
#include "debug.h"
#include "twi-v2.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define STATUS_MASK (TWSR & 0xF8)
/* #define CONTROL_MASK ((1 << TWEN) | (1 << TWIE)) */

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

enum {                          /* what we want to do */
      ACTION_WRITE,
      ACTION_READ,
      ACTION_IDLE,
};

static volatile uint8_t mode = MODE_IDLE;

static volatile uint8_t status = TWI_SUCCESS;

static volatile uint8_t m_action = ACTION_IDLE;
static volatile uint8_t m_id = TWI_ID_SIZE;
static volatile uint8_t m_reg = 0;
static volatile uint8_t m_data = 0;

static uint8_t m_address[TWI_ID_SIZE];
static twi_write_callback m_write_cb[TWI_ID_SIZE];
static twi_read_callback m_read_cb[TWI_ID_SIZE];

static struct buf_t queue;

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

uint8_t twi_slave (uint8_t id, uint8_t address,
                   twi_write_callback write_cb, twi_read_callback read_cb)
{
  if (id >= TWI_ID_SIZE)
    return 0;

  m_address[id] = address;
  m_write_cb[id] = write_cb;
  m_read_cb[id] = read_cb;

  return 1;
}

/* 1 is OK */
uint8_t twi_write_byte (uint8_t id, uint8_t reg, uint8_t value)
{
  if (id >= TWI_ID_SIZE)
    return 0;

  return ((buf_byte_fill (&queue, ACTION_WRITE) > 0)
          && (buf_byte_fill (&queue, id) > 0)
          && (buf_byte_fill (&queue, reg) > 0)
          && (buf_byte_fill (&queue, value) > 0))
    ? 1 : 0;
}

/* 1 is OK */
uint8_t twi_read_byte (uint8_t id, uint8_t reg)
{
  if (id >= TWI_ID_SIZE)
    return 0;

  return ((buf_byte_fill (&queue, ACTION_READ) > 0)
          && (buf_byte_fill (&queue, id) > 0)
          && (buf_byte_fill (&queue, reg) > 0)
          && (buf_byte_fill (&queue, 0) > 0))
    ? 1 : 0;
}

static uint8_t is_writing ()
{
  return (m_action == ACTION_WRITE) ? 1 : 0;
}

static uint8_t slave_address (uint8_t read)
{
  uint8_t addr = (m_address[m_id] << 1) | ((read != 0) ? 1 : 0);
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
      status = TWI_START_ERROR;
      stop ();
    } else {
      TWDR = slave_address (0);         /* slave addr, writing */
      /* TWCR |= (1 << TWEN); */
      TWCR &= ~(1 << TWSTA);
    }
    break;
  case MODE_WRITE_SLAVE:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 80); */
    if (check_status_register (TW_MT_SLA_ACK) == 0) {
      debug_1 (DEBUG_TWI, 4, STATUS_MASK);
      status = TWI_SLAVE_ERROR;
      stop ();
    } else {
      /* encode_msg_1 (MSG_ID_DEBUG_G, SERIAL_ID_TO_IGNORE, buf[0]); */
      /* TWCR &= ~(1 << TWSTA); */
      TWDR = m_reg;
    }
    break;
  case MODE_WRITE_REG:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90); */
    if (check_status_register (TW_MT_DATA_ACK) == 0) {
      /* encode_msg_2 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90, 1); */
      debug_1 (DEBUG_TWI, 5, STATUS_MASK);
      /* either reg error or value error */
      status = TWI_REG_ERROR;
      stop ();
    } else {
      if (is_writing () != 0) {
        /* encode_msg_2 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90, 3); */
        TWDR = m_data;
      } else {
        /* encode_msg_2 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 90, 2); */
        start (MODE_READ_START);
      }
    }
    break;
  case MODE_WRITE_VALUE:
    /* encode_msg_1 (MSG_ID_DEBUG_O, SERIAL_ID_TO_IGNORE, STATUS_MASK); */
    if (check_status_register (TW_MT_DATA_ACK) == 0) {
      /* encode_msg_1 (MSG_ID_DEBUG_O, SERIAL_ID_TO_IGNORE, 11); */
      status = TWI_VALUE_ERROR;
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
      status = TWI_START_ERROR;
      stop ();
    } else {
      /* TWCR &= ~(1 << TWSTA); */
      TWDR = slave_address (1);         /* slave addr, reading */
    }
    break;
  case MODE_READ_SLAVE:
    /* encode_msg_1 (MSG_ID_DEBUG_X, SERIAL_ID_TO_IGNORE, 110); */
    if (check_status_register (TW_MR_SLA_ACK) == 0) {
      debug_1 (DEBUG_TWI, 6, STATUS_MASK);
      status = TWI_SLAVE_ERROR;
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
      status = TWI_VALUE_ERROR;
      stop ();
    } else {
      m_data = TWDR;
      TWCR &= ~(1 << TWEA);   /* send nack - do we need this ? */
    }
    break;
  case MODE_READ_DONE:
    if (check_status_register (TW_MR_DATA_NACK) == 0) {
      debug_1 (DEBUG_TWI, 8, STATUS_MASK);
      status = TWI_STOP_ERROR;
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

static void action_reset ()
{
  status = TWI_SUCCESS;
  m_action = ACTION_IDLE;
  m_id = TWI_ID_SIZE;
  m_reg = 0;
  m_data = 0;
  for (uint8_t i = 0; i < TWI_ID_SIZE; ++i) {
    m_address[i] = 0;
    m_write_cb[i] = NULL;
    m_read_cb[i] = NULL;
  }
}

void twi_init ()
{
  mode = MODE_IDLE;
  action_reset ();
  buf_init (&queue);

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

static void action_set ()
{
  uint8_t action = ACTION_IDLE;
  uint8_t id = TWI_ID_SIZE;
  uint8_t reg = 0;
  uint8_t data = 0;
  if ((buf_byte_drain (&queue, &action) > 0)
      && (buf_byte_drain (&queue, &id) > 0)
      && (buf_byte_drain (&queue, &reg) > 0)
      && (buf_byte_drain (&queue, &data) > 0)) {
    m_action = action;
    m_id = id;
    m_reg = reg;
    m_data = data;
  }
}

void twi_try ()
{
  if (mode != MODE_IDLE)
    /* we are doing something */
    return;

  if (m_action != ACTION_IDLE) {
    /* we need to report result */
    if (m_action == ACTION_WRITE) {
      m_write_cb[m_id] (status);
    } else { /* READ */
      m_read_cb[m_id] (status, m_data);
    }
    action_reset ();
    return;
  }

  /* do we need to start new r/w ? */
  if (buf_size (&queue) > 0) {
    action_set ();
    start ((m_action == ACTION_WRITE) ? MODE_WRITE_START : MODE_READ_START);
  }
}
