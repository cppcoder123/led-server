
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/twi.h>

#include "twi.h"

#define STATUS_MASK (TWSR & 0xF8)

#define SLAVE_ADDRESS 0x16      /* addr selection wire to GND */

enum {
  mode_start,
  mode_slave,
  mode_reg,
  mode_value,
  mode_restart,
  mode_restart_slave,
  mode_restart_value,           /* read reg value */
  mode_stop,
  mode_idle,
  mode_last = mode_idle
};
static volatile uint8_t mode = mode_idle;

static volatile uint8_t status = TWI_SUCCESS;
static volatile uint8_t buf[2];

static twi_write_callback write_cb;
static twi_read_callback read_cb;


void twi_init ()
{
  write_cb = 0;
  read_cb = 0;

  /* bit rate: scl rate should 4*10^6 / (16 + 2 * TWBR * (4 ^TWPS)) */
  /* so 4*10^6 / (16 + 2 * 0x08 * 1)  = aprox 102kHz */
  TWBR = 0x08;
  /* enable twi, enable twi interrupt */
  TWCR |= (1 << TWEN) | (1 << TWIE);
}

static uint8_t busy ()
{
  return ((write_cb != 0) || (read_cb != 0)) ? 1 : 0;
}

void twi_try ()
{
  if ((mode != mode_idle)
      || (busy () == 0))
    return;

  if (write_cb) {
    write_cb (status);
    write_cb = 0;
  } else if (read_cb) {
    read_cb (status, buf[1]);
    read_cb = 0;
  }
}

static void start ()
{
  mode = mode_start;

  TWCR |= (1 << TWSTA);
}

uint8_t twi_write_reg (uint8_t reg, uint8_t value, twi_write_callback cb)
{
  if (busy () != 0)
    return 0;

  write_cb = cb;

  status = TWI_SUCCESS;

  buf[0] = reg;
  buf[1] = value;

  start ();

  return 1;
}

uint8_t twi_read_reg (uint8_t reg, twi_read_callback cb)
{
  if (busy () != 0)
    return 0;

  read_cb = cb;

  status = TWI_SUCCESS;

  buf[0] = reg;

  start ();
  
  return 1;
}

static uint8_t writing ()
{
  return (write_cb != 0) ? 1 : 0;
}

static void stop ()
{
  TWCR |= (1 << TWSTO);
  mode = mode_idle;
}

static uint8_t slave (uint8_t read)
{
  return (SLAVE_ADDRESS << 1) | ((read != 0) ? 1 : 0);
}

static void restart ()
{
  TWCR |= (1 << TWSTA);
}

ISR (TWI_vect)
{
  uint8_t mode_old = mode;
  if (mode < mode_idle)
    ++mode;

  switch (mode_old) {
  case mode_start:
    if (STATUS_MASK != TW_START) {
      status = TWI_START_FAILURE;
      stop ();
    } else {
      TWDR = slave (0);         /* slave addr, writing */
    }
    break;
  case mode_slave:
    if (STATUS_MASK != TW_MT_SLA_ACK) {
      status = TWI_START_SLAVE_FAILURE;
      stop ();
    } else {
      TWDR = buf[0];            /* register */
    }
    break;
  case mode_reg:
    if (STATUS_MASK != TW_MT_DATA_ACK) {
      status = TWI_START_DATA_1_FAILURE;
      stop ();
    } else {
      if (writing () != 0)
        TWDR = buf[1];
      else
        restart ();
    }
    break;
  case mode_value:
    if (STATUS_MASK != TW_MT_DATA_ACK)
      status = TWI_START_DATA_2_FAILURE;
    stop ();
    break;
  case mode_restart:
    if (STATUS_MASK != TW_REP_START) {
      status = TWI_RESTART_FAILURE;
      stop ();
    } else {
      TWDR = slave (1);         /* slave addr, reading */
    }
    break;
  case mode_restart_slave:
    if (STATUS_MASK != TW_MR_SLA_ACK) {
      status = TWI_RESTART_SLAVE_FAILURE;
      stop ();
    }
    break;
  case mode_restart_value:
    if (STATUS_MASK != TW_MR_DATA_ACK)
      status = TWI_RESTART_DATA_FAILURE;
    else
      buf[1] = TWDR;            /* reg value */
    stop ();
    break;
  case mode_stop:
    /* is it possible to get here? not sure */
    mode = mode_idle;
    break;
  default:
    break;
  }

  TWCR |= (1 << TWINT);         /* interrupt is handled by writing 1 ???*/
}
