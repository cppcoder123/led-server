/*
 *
 */

#include <avr/interrupt.h>

#include "button.h"
#include "buzz.h"
#include "decode.h"
#include "flush.h"
#include "flush-hw.h"
#include "power.h"
#include "spi.h"
#include "timer.h"

static void init ()
{
  button_init ();
  buzz_init ();
  decode_init ();
  flush_hw_init ();
  flush_init ();
  power_init ();
  spi_init ();

  /* fixme: it should be switchable, for now it is for r-pi only */
  timer_enable (TIMER_TEN_PER_SECOND, &flush_enable_shift);

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
