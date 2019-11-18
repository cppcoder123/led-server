/*
 *
 */

/* #define FREQUENCY 16000000UL */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "unix/constant.h"

#include "display.h"
#include "encode.h"

/* Achtung: Update also direction setup !*/
#define CS PORTA2
#define CLK PORTA6
#define DATA PORTA4

#define MASK(x) (1 << x)
#define RAISE(x) (PORTA |= MASK(x))
#define FALL(x) (PORTA &= ~MASK(x))

/*fixme: it looks that delay is so small, can we avoid it?*/
#define DELAY _delay_us (1)

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
  RAISE (CS);       \
  DELAY;            \
  FALL (CS);        \
  DELAY;

#define CS_DEACTIVATE \
  RAISE (CS);         \
  DELAY;

static void flush_bit (uint8_t bit)
{
  FALL (CLK);
  DELAY;
  if (bit)
    RAISE (DATA);
  else
    FALL (DATA);
  DELAY;
  RAISE (CLK);
  DELAY;
}

#define FLUSH_PREFIX(x)                         \
  {                                             \
    uint8_t i;                                  \
    for (i = 3; i > 0; --i)                     \
      flush_bit ((x) & (1 << (i - 1)));         \
  }

/*9 bits, last one is zero*/
#define FLUSH_CMD(x)                            \
  {                                             \
    uint8_t i;                                  \
    for (i = 8; i > 0; --i)                     \
      flush_bit ((x) & (1 << (i -1)));          \
    flush_bit (0);                              \
  }

#define FLUSH_ZERO_ADDRESS                      \
  {                                             \
    uint8_t i;                                  \
    for (i = 0; i < 7; ++i)                     \
      flush_bit (0);                            \
  }

/* ! backward order : low bit first */
#define FLUSH_HALF_BYTE(x)                      \
  {                                             \
     uint8_t i = 0;                             \
     for (i = 0; i < 4; ++i)                    \
       flush_bit ((x) & (1 << i));              \
  }

void display_init ()
{
  /*fixme*/
  DDRA |= MASK (CS) | MASK (CLK) | MASK (DATA);

  /*debug: it should be called outside: fixme */
  display_start ();
}

void display_start ()
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

  display_mono_start ();
  for (uint8_t i = 0; i < 32; ++i)
    display_mono (0xAA);
  display_mono_stop ();
}

void display_stop ()
{
  /*fixme*/
  CS_ACTIVATE;
  FLUSH_PREFIX (PREFIX_CMD);
  FLUSH_CMD (CMD_DISABLE);
  CS_DEACTIVATE;
}

void display_mono_start ()
{
  /* encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, 177); */
  /*fixme*/
  CS_ACTIVATE;
  FLUSH_PREFIX (PREFIX_DATA);
  FLUSH_ZERO_ADDRESS;
}

void display_mono (uint8_t data)
{
  /* encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, 199); */
  /*fixme*/
  FLUSH_HALF_BYTE (data & 0xF);
  uint8_t high = data & 0xF0;
  high >>= 4;
  FLUSH_HALF_BYTE (high);
}

void display_mono_stop ()
{
  /*fixme*/
  CS_DEACTIVATE;
}

void display_brightness (uint8_t value)
{
  CS_ACTIVATE;
  FLUSH_PREFIX (PREFIX_CMD);
  FLUSH_CMD (CMD_BRIGHTNESS_MASK | (value & 0xF));
  CS_DEACTIVATE;
}
