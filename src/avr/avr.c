/*
 *
 */

#include <avr/interrupt.h>

/* #include "buzz.h" */
#include "counter.h"
#include "decode.h"
#include "flush.h"
#include "postpone.h"
#include "power.h"
#include "rotor.h"
#include "spi.h"
#include "sync.h"

static void init ()
{
  /*init channel first*/
  spi_init ();

  /* buzz_init (); */
  /* debug_init (); */

  /* init counters right after spi */
  counter_init ();
  decode_init ();
  flush_init ();
  postpone_init ();
  rotor_init ();
  power_init ();
  sync_init ();

  /* enable r-pi */
  power_up ();

  sei ();
}

int main ()
{
  init ();

  while (1) {
    /* buzz_try (); */
    decode_try ();
    flush_try ();
    postpone_try ();
    rotor_try ();
    sync_try ();
  }

  return 0;
}
