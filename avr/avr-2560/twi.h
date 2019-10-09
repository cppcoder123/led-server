/*
 *
 */

#ifndef TWI_H
#define TWI_H

#include <stdint.h>

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
