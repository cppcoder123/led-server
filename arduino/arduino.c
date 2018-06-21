/*
 *
 *
 *
 *
 *
 */

#include <avr/interrupt.h>

#include <stdint.h>

#include "device-id.h"

#include "button.h"
#include "codec.h"
#include "matrix-buffer.h"
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
  matrix_buffer_init ();
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
  self_test ();

  while (1) {
    /*
     * uart read/write, spi write and button handling
     * use interrupts, so we only need to parse uart read queue 
     */
    parse ();
  }

  return 0;
}
