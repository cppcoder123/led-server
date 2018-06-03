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

#include "codec.h"
#include "matrix.h"
#include "matrix-timer.h"
#include "parse.h"
#include "self-test.h"
#include "spi-write.h"
#include "uart-read.h"
#include "uart-write.h"

void hardware_init ()
{
  sei ();
}

void software_init ()
{
  codec_init ();
  matrix_init ();
  matrix_timer_init ();
  parse_init ();
  spi_write_init ();
  uart_write_init ();
  uart_read_init ();
}

static void say_hello ()
{
  codec_encode_1 (ID_STATUS, 0, ID_STATUS_HELLO);
}

int main ()
{
  hardware_init ();
  software_init ();
  self_test ();
  say_hello ();

  while (1) {
    /*
     * uart read/write, spi write and button handling
     * use interrupts, so we only need to parse uart read queue 
     */
    parse ();
  }

  return 0;
}
