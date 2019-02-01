/*
 *
 */

/* #define FREQUENCY 16000000UL */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "flush-hw.h"

#define CS PL7
#define CLK PL6
#define DATA PL5

#define MSK(x) (1 << x)
#define RISE(x) (PORTL |= MSK(x))
#define FALL(x) (PORTL &= ~MSK(x))

/*fixme: it looks that delay is so small, can we avoid it?*/
#define DELAY _delay_us (0.1)

#define PREFIX_CMD 0x4          /* b100 */
#define PREFIX_DATA 0x5         /* b101 */

#define CMD_DISABLE 0x0
#define CMD_ENABLE 0x1
#define CMD_NMOS_8COM 0x20
#define CMD_LED_OFF 0x2
#define CMD_LED_ON 0x3
#define CMD_BRIGHTNESS_MASK 0xA0

/*we have !CS -> active low*/
#define CS_ACTIVATE \
  RISE (CS);        \
  DELAY;            \
  FALL (CS);        \
  DELAY;

#define CS_DEACTIVATE RISE (CS)

#define FLUSH_BIT(x) \
  FALL (CLK);       \
  if (x)            \
    RISE (DATA);    \
  else              \
    FALL (DATA);    \
  DELAY;            \
  RISE (CLK);       \
  DELAY

#define FLUSH_PREFIX(x)                         \
  {                                             \
    uint8_t i;                                  \
    for (i = 3; i > 0; --i)                     \
      FLUSH_BIT ((x) & (1 << (i - 1)));         \
  }

/*9 bits, last one is zero*/
#define FLUSH_CMD(x)                            \
  {                                             \
    uint8_t i;                                  \
    for (i = 8; i > 0; --i)                     \
      FLUSH_BIT ((x) & (1 << (i -1)));          \
    FLUSH_BIT (0);                              \
  }

#define FLUSH_ZERO_ADDRESS                      \
  {                                             \
    uint8_t i;                                  \
    for (i = 0; i < 7; ++i)                     \
      FLUSH_BIT (0);                            \
  }
  
/* ! backward order : low bit first */
#define FLUSH_HALF_BYTE(x)                      \
  {                                             \
     uint8_t i = 0;                             \
     for (i = 0; i < 4; ++i)                    \
       FLUSH_BIT ((x) & (1 << i));              \
  }

void flush_hw_init ()
{
  /*fixme*/
  flush_hw_start ();
}

void flush_hw_start ()
{
  /*fixme*/
  CS_ACTIVATE;
  FLUSH_PREFIX (PREFIX_CMD);
  FLUSH_CMD (CMD_DISABLE);
  FLUSH_CMD (CMD_NMOS_8COM);
  FLUSH_CMD (CMD_ENABLE);
  FLUSH_CMD (CMD_LED_ON);
  FLUSH_CMD (CMD_BRIGHTNESS_MASK | 0xF);
  CS_DEACTIVATE;
}

void flush_hw_stop ()
{
  /*fixme*/
  CS_ACTIVATE;
  FLUSH_PREFIX (PREFIX_CMD);
  FLUSH_CMD (CMD_DISABLE);
  CS_DEACTIVATE;
}

void flush_hw_mono_start ()
{
  /*fixme*/
  CS_ACTIVATE;
  FLUSH_PREFIX (PREFIX_DATA);
  FLUSH_ZERO_ADDRESS;
}

void flush_hw_mono (data_t data)
{
  /*fixme*/
  FLUSH_HALF_BYTE (data & 0xF);
  uint8_t high = data & 0xF0;
  high >>= 4;
  FLUSH_HALF_BYTE (high);
}

void flush_hw_mono_stop ()
{
  /*fixme*/
  CS_DEACTIVATE;
}

void flush_hw_brightness (data_t value)
{
  CS_ACTIVATE;
  FLUSH_PREFIX (PREFIX_CMD);
  FLUSH_CMD (CMD_BRIGHTNESS_MASK | (value & 0xF));
  CS_DEACTIVATE;
}
