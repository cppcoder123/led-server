/*
 *
 */

#ifndef TWI_H
#define TWI_H

#include <stdint.h>

enum {
  TWI_SUCCESS,
  TWI_START_FAILURE,            /* write */
  TWI_START_SLAVE_FAILURE,
  TWI_START_DATA_1_FAILURE,
  TWI_START_DATA_2_FAILURE,
  TWI_RESTART_FAILURE,          /* read */
  TWI_RESTART_SLAVE_FAILURE,
  TWI_RESTART_DATA_FAILURE
};

typedef void (*twi_write_callback) (uint8_t /*status*/);
typedef void (*twi_read_callback) (uint8_t /*status*/, uint8_t /*value*/);

void twi_init ();

void twi_try ();

/*
 * Read/write from/to button handler register
 * Functions return 0 in case of failure
 */
uint8_t twi_write_reg (uint8_t reg, uint8_t value, twi_write_callback cb);
uint8_t twi_read_reg (uint8_t reg, twi_read_callback cb);

#endif
