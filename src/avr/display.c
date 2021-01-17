/*
 *
 */

/* #define FREQUENCY 16000000UL */
#define F_CPU 4000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "unix/constant.h"

#include "display.h"
/* #include "encode.h" */

#define DISPLAY_PORT PORTC
#define DISPLAY_DIRECTION_PORT DDRC

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

void display_brightness (struct display_t *display, uint8_t value)
{
  cs_activate (display->cs);
  flush_prefix (display->clk, display->data, PREFIX_CMD);
  flush_cmd (display->clk, display->data, CMD_BRIGHTNESS_MASK | (value & 0xF));
  cs_deactivate (display->cs);
}
