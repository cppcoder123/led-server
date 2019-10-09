//
//
//
#include "frequency.h"

#define F_CPU FREQUENCY

#include <avr/io.h>
#include <util/delay.h>

#include "ht1632c.h"


// 
// LED communication macros (fixme)
// 

#define LEFT_MATRIX_SLAVE_SELECT PC0
#define LEFT_MATRIX_CLOCK PC1
#define LEFT_MATRIX_DATA PC2

#define RIGHT_MATRIX_SLAVE_SELECT PC3
#define RIGHT_MATRIX_CLOCK PC4
#define RIGHT_MATRIX_DATA PC5

#define SLAVE_SELECT(matrix) matrix##_SLAVE_SELECT
#define CLOCK(matrix) matrix##_CLOCK
#define DATA(matrix) matrix##_DATA

#define BIT_SHIFT(distance) (1 << distance)

#define SLAVE_SELECT_ACTIVATE(matrix) PORTC &= ~BIT_SHIFT (SLAVE_SELECT (matrix))
#define SLAVE_SELECT_RELAX(matrix) PORTC |= BIT_SHIFT (SLAVE_SELECT (matrix))

#define CLOCK_RELAX(matrix) PORTC &= ~BIT_SHIFT (CLOCK (matrix))
#define CLOCK_ACTIVATE(matrix) PORTC |= BIT_SHIFT (CLOCK (matrix))

#define DATA_LOW(matrix) PORTC &= ~BIT_SHIFT (DATA (matrix)) 
#define DATA_HIGH(matrix) PORTC |= BIT_SHIFT (DATA (matrix))

//
//
//
#define SELECT_HOLD _delay_us (0.1)
#define DATA_HOLD _delay_us (0.1)

//
#define SLAVE_SELECT_PULSE(matrix)              \
  SLAVE_SELECT_RELAX (matrix);                  \
  SELECT_HOLD;                                  \
  SLAVE_SELECT_ACTIVATE (matrix);               \
  SELECT_HOLD


//
// LED related data should be written in low-high bit order,
// everything else (command, address, ...) in high-low order :(
//

#define WRITE_DATA(matrix, data, start, finish)                 \
  {                                                             \
    uint8_t mask = 1 << start;                                  \
    uint8_t finish_mask = 1 << finish;                          \
    while (1) {                                                 \
      CLOCK_RELAX (matrix);                                     \
      if (data & mask)                                          \
        DATA_HIGH (matrix);                                     \
      else                                                      \
        DATA_LOW (matrix);                                      \
      DATA_HOLD;                                                \
      CLOCK_ACTIVATE (matrix);                                  \
      DATA_HOLD;                                                \
      if (mask == finish_mask)                                  \
        break;                                                  \
      if (start < finish)                                       \
        mask <<= 1;                                             \
      else  if (start > finish)                                 \
        mask >>= 1;                                             \
      else                                                      \
        break;                                                  \
    };                                                          \
  }

// write prefix : command or data
#define WRITE_PREFIX(matrix, prefix)  WRITE_DATA (matrix, prefix, 2, 0)

// 
#define WRITE_COMMAND(matrix, command)            \
  WRITE_DATA (matrix, command, 7, 0);             \
  WRITE_DATA (matrix, 0, 0, 0)

#define WRITE_ADDRESS(matrix, address)          \
  WRITE_DATA (matrix, address, 6, 0)

#define WRITE_WORD(matrix, word)                \
  WRITE_DATA (matrix, word, 0, 3)

//
// Prefixes
// 

// 0b100
#define COMMAND_PREFIX 0x4
// 0b101
#define DATA_PREFIX 0x5 

//
// Commands
//
#define COMMAND_SYSTEM_DISABLE 0x0
#define COMMAND_SYSTEM_ENABLE 0x1
#define COMMAND_NMOS_8COM 0x20  /* configure led connection */
#define COMMAND_LED_OFF 0x2     /* turn off led duty cycle generator */
#define COMMAND_LED_ON 0x3      /* turn on led duty cycle generator */
#define COMMAND_BRIGHTNESS_MASK 0xA0

#define START_MATRIX(matrix)                                    \
  SLAVE_SELECT_PULSE (matrix);                                  \
  WRITE_PREFIX (matrix, COMMAND_PREFIX);                        \
  WRITE_COMMAND (matrix, COMMAND_SYSTEM_DISABLE);               \
  WRITE_COMMAND (matrix, COMMAND_NMOS_8COM);                    \
  WRITE_COMMAND (matrix, COMMAND_SYSTEM_ENABLE);                \
  WRITE_COMMAND (matrix, COMMAND_LED_ON);                       \
  WRITE_COMMAND (matrix, (COMMAND_BRIGHTNESS_MASK | 0xF));      \
  SLAVE_SELECT_RELAX (matrix);                                  \
  SELECT_HOLD
// fixme: do we need to clear matrix here?

#define STOP_MATRIX(matrix)                             \
  SLAVE_SELECT_PULSE (matrix);                          \
  WRITE_PREFIX (matrix, COMMAND_PREFIX);                \
  WRITE_COMMAND (matrix, COMMAND_SYSTEM_DISABLE);       \
  SLAVE_SELECT_RELAX (matrix);                          \
  SELECT_HOLD

#define BRIGHTNESS_MATRIX(matrix, level)        \
  SLAVE_SELECT_PULSE (matrix);                  \
  WRITE_PREFIX (matrix, COMMAND_PREFIX);        \
  WRITE_COMMAND (matrix, level);                \
  SLAVE_SELECT_RELAX (matrix);                  \
  SELECT_HOLD

// 7 bit address & 8 bit data (4 bits 2 times)
#define DATA_MATRIX(matrix, address, word_low, word_high) \
  SLAVE_SELECT_PULSE (matrix);                            \
  WRITE_PREFIX (matrix, DATA_PREFIX);                     \
  WRITE_ADDRESS (matrix, address);                        \
  WRITE_WORD (matrix, word_low);                          \
  WRITE_WORD (matrix, word_high);                         \
  SLAVE_SELECT_RELAX (matrix);                            \
  SELECT_HOLD

void ht1632c_start ()
{
  START_MATRIX (LEFT_MATRIX);
  START_MATRIX (RIGHT_MATRIX);
}

void ht1632c_stop ()
{
  STOP_MATRIX (LEFT_MATRIX);
  STOP_MATRIX (RIGHT_MATRIX);
}

void ht1632c_brightness (uint8_t level)
{
  uint8_t command = COMMAND_BRIGHTNESS_MASK | (0xF & level);
  BRIGHTNESS_MATRIX (LEFT_MATRIX, command);
  BRIGHTNESS_MATRIX (RIGHT_MATRIX, command);
}

void ht1632c_data (uint8_t column, uint8_t left_data, uint8_t right_data)
{
  // column should not be more than 31
  uint8_t address = 2 * (column & 0x1F);

  uint8_t word_low = left_data & 0xF;
  uint8_t word_high = left_data & 0xF0;
  word_high >>= 4;
  DATA_MATRIX (LEFT_MATRIX, address, word_low, word_high);

  word_low = right_data & 0xF;
  word_high = right_data & 0xF0;
  word_high >>= 4;
  DATA_MATRIX (RIGHT_MATRIX, address, word_low, word_high);
}
