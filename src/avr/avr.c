/*
 *
 */

#include <avr/interrupt.h>

/* #include "buzz.h" */
#include "clock.h"
#include "counter.h"
#include "decode.h"
#include "flush.h"
#include "postpone.h"
#include "power.h"
#include "rotor.h"
#include "spi.h"

static void init ()
{
  /*init channel first*/
  spi_init ();

  /* buzz_init (); */
  /* debug_init (); */
  clock_init ();
  counter_init ();
  decode_init ();
  flush_init ();
  postpone_init ();
  rotor_init ();
  power_init ();

  /* enable r-pi */
  power_up ();

  sei ();
}

int main ()
{
  init ();

  while (1) {
    /* buzz_try (); */
    clock_try ();
    decode_try ();
    flush_try ();
    rotor_try ();
    postpone_try ();
  }

  return 0;
}
