/*
 *
 */

#include <avr/interrupt.h>

#include "bitbang.h"
#include "button.h"
#include "buzz.h"
#include "decode.h"
#include "debug.h"
#include "flush.h"
#include "postpone.h"
#include "power.h"
#include "spi.h"

static void init ()
{
  /*init channel first*/
  spi_init ();

  button_init ();
  buzz_init ();
  debug_init ();
  decode_init ();
  bitbang_init ();
  flush_init ();
  postpone_init ();
  power_init ();

  /* enable r-pi */
  power_up ();

  sei ();
}

int main ()
{
  init ();

  while (1) {
    buzz_try ();
    decode_try ();
    flush_try ();
    postpone_try ();
  }

  return 0;
}
