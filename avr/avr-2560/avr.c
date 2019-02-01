/*
 *
 */

#include <avr/interrupt.h>

#include "button.h"
#include "buzz.h"
#include "decode.h"
#include "flush.h"
#include "flush-hw.h"
#include "flush-timer.h"
#include "power.h"
#include "spi.h"

static void init ()
{
  button_init ();
  buzz_init ();
  decode_init ();
  flush_hw_init ();
  flush_init ();
  flush_timer_init ();
  power_init ();
  spi_init ();

  sei ();
}

int main ()
{
  init ();

  while (1) {
    buzz_try ();
    decode_try ();
    flush_try ();
  }

  return 0;
}
