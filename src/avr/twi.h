/*
 * Handle I2C communication
 */

#ifndef TWI_H
#define TWI_H

#include <stdint.h>

enum {                          /* read/write status */
      TWI_SUCCESS,
      //
      TWI_WRITE_START_ERROR,
      TWI_READ_START_ERROR,
      TWI_READ_RESTART_ERROR,
      //
      TWI_WRITE_SLAVE_ERROR,
      TWI_READ_SLAVE_ERROR,
      TWI_READ_RESTART_SLAVE_ERROR,
      //
      TWI_WRITE_REG_ERROR,
      TWI_READ_REG_ERROR,
      //
      TWI_WRITE_VALUE_ERROR,
      TWI_READ_VALUE_ERROR,
      //
};

enum {                          /* slave ids */
      TWI_ID_RTC,
      TWI_ID_SIZE,              /* keep last ! */
};

typedef void (*twi_write_callback) (uint8_t /*tag*/, uint8_t /*status*/);
typedef void (*twi_read_callback) (uint8_t /*tag*/, uint8_t /*status*/,
                                   uint8_t /*len*/,
                                   volatile uint8_t* /*value*/);

/*
 *
 * Functions return 0 in case of failure
 *
 *
 * Define slave address & r/w callbacks
 *
 * twi_slave    -> no callbacks will be called, suitable for writing
 *                 no status check is required
 * twi_slave_r  -> read & no write status check
 * twi_slave_rw -> read & write status check
 *
 */
uint8_t twi_slave (uint8_t id,      /* TWI_ID_XXX */
                   uint8_t address /* slave address */);
uint8_t twi_slave_r (uint8_t id,      /* TWI_ID_XXX */
                     uint8_t address, /* slave address */
                     twi_read_callback read_cb);
uint8_t twi_slave_rw (uint8_t id,      /* TWI_ID_XXX */
                      uint8_t address, /* slave address */
                      twi_write_callback write_cb, twi_read_callback read_cb);

/*
 * 'tag' is an abitrary value that will be returned in callback
 *
 *
 * Write one byte to the register
 */
uint8_t twi_write_byte (uint8_t id, uint8_t tag, uint8_t reg, uint8_t value);

/*
 * Read one byte from the register
 */
uint8_t twi_read_byte (uint8_t id, uint8_t tag, uint8_t reg);

/*
 * Write array starting with register
 */
uint8_t twi_write_array (uint8_t id, uint8_t tag, uint8_t len /*1-128*/,
                         uint8_t reg, uint8_t *data);
/*
 * Read array starting with register
 */
uint8_t twi_read_array (uint8_t id, uint8_t tag, uint8_t len /*1-128*/, uint8_t reg);

void twi_init ();

void twi_try ();

#endif
