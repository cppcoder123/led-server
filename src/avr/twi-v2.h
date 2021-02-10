/*
 * Handle I2C communication
 *   (Note: arrays are not implemented)
 */

#ifndef TWI_H
#define TWI_H

#include <stdint.h>

enum {                          /* read/write status */
      TWI_SUCCESS,
      TWI_START_ERROR,
      TWI_SLAVE_ERROR,
      TWI_REG_ERROR,
      TWI_VALUE_ERROR,
      TWI_STOP_ERROR,
};

enum {                          /* slave ids */
      TWI_ID_RTC,
      TWI_ID_DAC,
      TWI_ID_SIZE,              /* keep last ! */
};

typedef void (*twi_write_callback) (uint8_t /*status*/);
typedef void (*twi_read_callback) (uint8_t /*status*/, uint8_t /*value*/);

/*
 *
 * Functions return 0 in case of failure
 *
 *
 * Define slave address & r/w callbacks
 */
uint8_t twi_slave (uint8_t id,      /* TWI_ID_XXX */
                   uint8_t address, /* slave address */
                   twi_write_callback write_cb, twi_read_callback read_cb);

/*
 * Write one byte to the register
 */
uint8_t twi_write_byte (uint8_t id, uint8_t reg, uint8_t value);

/*
 * Read one byte from the register
 */
uint8_t twi_read_byte (uint8_t id, uint8_t reg);

void twi_init ();

void twi_try ();

#endif