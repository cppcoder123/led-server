/*
 *
 */

#include <avr/interrupt.h>

//#include "buzz.h"
#include "decode.h"
#include "debug.h"
#include "display.h"
#include "flush.h"
#include "postpone.h"
#include "rotor.h"
#include "spi.h"

static void init ()
{
  /*init channel first*/
  spi_init ();

  /* buzz_init (); */
  /* debug_init (); */
  decode_init ();
  display_init ();
  flush_init ();
  postpone_init ();
  rotor_init ();
  /* power_init (); */

  /* enable r-pi */
  /* power_up (); */

  sei ();
}

int main ()
{
  init ();

  while (1) {
    /* buzz_try (); */
    decode_try ();
    /* flush_try (); */
    rotor_try ();
    postpone_try ();
  }

  return 0;
}