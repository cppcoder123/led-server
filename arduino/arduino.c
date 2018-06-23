/*
 *
 *
 *
 *
 *
 */

#include <avr/interrupt.h>

#include <stdint.h>

/* #include "button.h" */
#include "codec.h"
#include "parse.h"
#include "render.h"
#include "self-test.h"
#include "spi-write.h"
#include "uart.h"
#include "uart-write.h"

static void init ()
{
  /* button_init (); */
  codec_init ();
  parse_init ();
  render_init ();
  spi_write_init ();
  uart_init ();
  /* enable interrupts */
  sei ();
}

int main ()
{
  init ();
  self_test (0xFF);

  while (1) {
    /*
     * uart read/write, spi write and button handling
     * use interrupts, so we only need to parse uart read queue 
     */
    /* self_test (0x44); */
    parse ();
    /* self_test (0xAA); */
  }

  return 0;
}
