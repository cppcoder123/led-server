/*
 *
 */
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/twi.h>

#include "buf.h"
#include "twi.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define STATUS_MASK (TWSR & 0xF8)

#define FOR_WRITE 0
#define FOR_READ 1

#define GO_AHEAD ((1 << TWINT) | (1 << TWEN) | (1 << TWIE))

enum {
      MODE_WRITE_START,         /* W: start writing */
      MODE_WRITE_SLAVE,         /* W: slave device addr, w */
      MODE_WRITE_REG,           /* W: slave register addr */
      MODE_WRITE_VALUE,         /* W: slave register value */
      MODE_READ_START,          /* R: start reading */
      MODE_READ_SLAVE,          /* R: slave addr, w */
      MODE_READ_REG,            /* R: slave addr reg value, w */
      MODE_READ_RESTART,        /* R: restart in r mode */
      MODE_READ_RESTART_SLAVE,  /* R: restart in r mode */
      MODE_READ_VALUE,          /* R: slave read value */
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

  /* the same for read and write */
  TWCR = GO_AHEAD | (1 << TWSTA);
}

static void stop ()
{
  TWCR = GO_AHEAD | (1 << TWSTO);

  /* there is no interrupt call after stop => set idle mode manually */
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

static uint8_t slave_address (uint8_t action)
{
  return (m_address[m_id] << 1) | ((action == FOR_READ) ? 1 : 0);
}

static uint8_t check_status_register (uint8_t mask)
{
  return ((STATUS_MASK & mask) == mask) ? 1 : 0;
}

ISR (TWI_vect)
{
  uint8_t mode_old = mode;
  if (mode < MODE_IDLE)
    ++mode;


  switch (mode_old) {
  case MODE_WRITE_START:
    if (check_status_register (TW_START) == 0) {
      status = TWI_WRITE_START_ERROR;
      stop ();
    } else {
      TWDR = slave_address (FOR_WRITE); /* slave addr, writing */
      TWCR = GO_AHEAD;
    }
    break;
  case MODE_WRITE_SLAVE:
    if (check_status_register (TW_MT_SLA_ACK) == 0) {
      status = TWI_WRITE_SLAVE_ERROR;
      stop ();
    } else {
      TWDR = m_reg;
      TWCR = GO_AHEAD;
    }
    break;
  case MODE_WRITE_REG:
    if (check_status_register (TW_MT_DATA_ACK) == 0) {
      /* either reg error or value error */
      status = TWI_WRITE_REG_ERROR;
      stop ();
    } else {
      TWDR = m_data;
      TWCR = GO_AHEAD;
    }
    break;
  case MODE_WRITE_VALUE:
    if (check_status_register (TW_MT_DATA_ACK) == 0) {
      status = TWI_WRITE_VALUE_ERROR;
      stop ();
    } else {
      stop ();
    }
    break;
  case MODE_READ_START:
    if (check_status_register (TW_START) == 0) {
      status = TWI_READ_START_ERROR;
      stop ();
    } else {
      /* we need to write register first */
      TWDR = slave_address (FOR_WRITE);
      TWCR = GO_AHEAD;
    }
    break;
  case MODE_READ_SLAVE:
    if (check_status_register (TW_MT_SLA_ACK) == 0) {
      status = TWI_READ_SLAVE_ERROR;
      stop ();
    } else {
      /* slave is OK we need to write reg */
      TWDR = m_reg;
      TWCR = GO_AHEAD;
    }
    break;
  case MODE_READ_REG:
    if (check_status_register (TW_MT_DATA_ACK) == 0) {
      status = TWI_READ_REG_ERROR;
      stop ();
    } else {
      /* send restart */
      TWCR = GO_AHEAD | (1 << TWSTA);
    }
    break;
  case MODE_READ_RESTART:
    if (check_status_register (TW_REP_START) == 0) {
      status = TWI_READ_RESTART_ERROR;
      stop ();
    } else {
      TWDR = slave_address (FOR_READ);
      TWCR = GO_AHEAD;
    }
    break;
  case MODE_READ_RESTART_SLAVE:
    if (check_status_register (TW_MR_SLA_ACK) == 0) {
      status = TWI_READ_RESTART_SLAVE_ERROR;
      stop ();
    } else {
      TWCR = GO_AHEAD;
    }
    break;
  case MODE_READ_VALUE:
    if (check_status_register (TW_MR_DATA_ACK) == 0) {
      status = TWI_READ_VALUE_ERROR;
      stop ();
    } else {
      m_data = TWDR;
      stop ();
    }
    break;
  case MODE_IDLE:
    /* is it possible to get here? not sure */
    mode = MODE_IDLE;
    break;
  default:
    break;
  }
}

static void action_reset ()
{
  status = TWI_SUCCESS;
  m_action = ACTION_IDLE;
  /* m_id = 0; either SIZE or 0 is dangerous :( */
  m_reg = 0;
  m_data = 0;
}

void twi_init ()
{
  mode = MODE_IDLE;
  action_reset ();
  for (uint8_t i = 0; i < TWI_ID_SIZE; ++i) {
    m_address[i] = 0;
    m_write_cb[i] = NULL;
    m_read_cb[i] = NULL;
  }
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
