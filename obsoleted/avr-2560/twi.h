/*
 *
 */

#ifndef TWI_H
#define TWI_H

#include <stdint.h>

#define TWI_WORD_SIZE 8

enum {
  TWI_SUCCESS,
  TWI_WRITE_START_ERROR,
  TWI_WRITE_SLAVE_ERROR,
  TWI_WRITE_REG_ERROR,
  TWI_WRITE_VALUE_ERROR,
  TWI_READ_START_ERROR,
  TWI_READ_SLAVE_ERROR,
  TWI_READ_VALUE_ERROR,
  TWI_READ_DONE_ERROR,
  /* --- */
  TWI_WRITE_BYTE_ERROR,
  TWI_WRITE_WORD_ERROR,
  TWI_READ_BYTE_ERROR,
  TWI_READ_WORD_ERROR,
};

typedef void (*twi_write_callback) (uint8_t /*status*/);
typedef void (*twi_read_callback) (uint8_t /*status*/,
                                   volatile uint8_t* /*value*/);

void twi_init ();

void twi_try ();

/*
 * Read/write from/to button handler 
 * Functions return 0 in case of failure
 */

/* read byte from specified register */
uint8_t twi_read_byte (uint8_t reg, twi_read_callback cb);

/* read word starting @reg */
/* a) buf should be at least 8 bytes long */
/* b) only status at 'read_callback' make sense here */
uint8_t twi_read_word (uint8_t reg, twi_read_callback cb);

/* write one byte to specified register */
uint8_t twi_write_byte (uint8_t reg, uint8_t value, twi_write_callback cb);

/* It looks this function has bug(s), some efforts needed */
/* write one word (8 byte) to specified register */
uint8_t twi_write_word (uint8_t reg, uint8_t value, twi_write_callback cb);

/* debug fun */
void twi_debug_cb ();

#endif
