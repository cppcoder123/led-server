/*
 *
 */

/* #define FREQUENCY 16000000UL */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "unix/constant.h"

#include "display.h"
/* #include "encode.h" */

#define DISPLAY_PORT PORTA
#define DISPLAY_DIRECTION_PORT DDRA

#define CMD_DISABLE 0x0
#define CMD_ENABLE 0x1
#define CMD_NMOS_8COM 0x20
/* #define CMD_LED_OFF 0x2 */
#define CMD_LED_ON 0x3
#define CMD_BRIGHTNESS_MASK 0xA0

/*fixme: it looks that delay is so small, can we avoid it?*/
#define DELAY _delay_us (1)

#define PREFIX_CMD 0x4          /* b100 */
#define PREFIX_DATA 0x5         /* b101 */


static void cs_activate (uint8_t cs)
{
  DISPLAY_PORT |= cs;           /* rise */
  DELAY;
  DISPLAY_PORT &= ~cs;          /* fall */
  DELAY;
}

static void cs_deactivate (uint8_t cs)
{
  DISPLAY_PORT |= cs;
  DELAY;
}

static void flush_bit (uint8_t clk, uint8_t data, uint8_t bit)
{
  DISPLAY_PORT &= ~clk;
  DELAY;
  if (bit)
    DISPLAY_PORT |= data;
  else
    DISPLAY_PORT &= ~data;
  DELAY;
  DISPLAY_PORT |= clk;
  DELAY;
}

static void flush_prefix (uint8_t clk, uint8_t data, uint8_t prefix)
{
  uint8_t mask = (1 << 2);
  for (uint8_t i = 0; i < 3; ++i) { /* 3 bits */
    flush_bit (clk, data, prefix & mask);
    mask >>= 1;
  }
}

static void flush_zero_address (uint8_t clk, uint8_t data)
{
  for (uint8_t i = 0; i < 7; ++i)
    flush_bit (clk, data, 0);
}

static void flush_nibble (uint8_t clk, uint8_t data, uint8_t nibble)
{
  /* ! backward order : low bit first */
  uint8_t mask = (1 << 0);
  for (uint8_t i = 0; i < 4; ++i) {
    flush_bit (clk, data, nibble & mask);
    mask <<= 1;
  }
}

static void flush_cmd (uint8_t clk, uint8_t data, uint8_t cmd)
{
  /*9 bits, last one is zero*/
  uint8_t mask = (1 << 7);
  for (uint8_t i = 0; i < 8; ++i) {
    flush_bit (clk, data, cmd & mask);
    mask >>= 1;
  }
  flush_bit (clk, data, 0);
}

static void start (uint8_t cs, uint8_t clk, uint8_t data)
{
  cs_activate (cs);
  flush_prefix (clk, data, PREFIX_CMD);
  flush_cmd (clk, data, CMD_DISABLE);
  flush_cmd (clk, data, CMD_NMOS_8COM);
  flush_cmd (clk, data, CMD_ENABLE);
  flush_cmd (clk, data, CMD_LED_ON);
  flush_cmd (clk, data, CMD_BRIGHTNESS_MASK | 0xF);
  cs_deactivate (cs);
}

void display_init (struct display_t *display,
                   uint8_t cs, uint8_t clk, uint8_t data)
{
  display->cs = (1 << cs);
  display->clk = (1 << clk);
  display->data = (1 << data);

  /* all of them are outputs */
  DISPLAY_DIRECTION_PORT |= (display->cs | display->clk | display->data);

  start (display->cs, display->clk, display->data);
}

void display_data_start (struct display_t *display)
{
  cs_activate (display->cs);
  flush_prefix (display->clk, display->data, PREFIX_DATA);
  flush_zero_address (display->clk, display->data);
}

void display_data_column (struct display_t *display, uint8_t column)
{
  flush_nibble (display->clk, display->data, column & 0xF);
  uint8_t high = column & 0xF0;
  flush_nibble (display->clk, display->data, high >> 4);
}

void display_data_stop (struct display_t *display)
{
  cs_deactivate (display->cs);
}

#if 0
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

  /* display_data_start (); */
  /* for (uint8_t i = 0; i < 32; ++i) */
  /*   display_data_column (0xAA); */
  /* display_data_stop (); */
}

void display_stop ()
{
  /*fixme*/
  CS_ACTIVATE;
  FLUSH_PREFIX (PREFIX_CMD);
  FLUSH_CMD (CMD_DISABLE);
  CS_DEACTIVATE;
}

void display_data_start ()
{
  /* encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, 177); */
  /*fixme*/
  CS_ACTIVATE;
  FLUSH_PREFIX (PREFIX_DATA);
  FLUSH_ZERO_ADDRESS;
}

void display_data_column (uint8_t data)
{
  /* encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, 199); */
  /*fixme*/
  FLUSH_HALF_BYTE (data & 0xF);
  uint8_t high = data & 0xF0;
  high >>= 4;
  FLUSH_HALF_BYTE (high);
}

void display_data_stop ()
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
#endif
