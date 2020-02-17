/*
 *
 */

#include <avr/interrupt.h>

#include "at.h"
#include "buzz.h"
#include "clock.h"
#include "counter.h"
#include "decode.h"
#include "flush.h"
#include "heartbeat.h"
#include "invoke.h"
#include "menu.h"
#include "mode.h"
#include "postpone.h"
#include "power.h"
#include "rotor.h"
#include "spi.h"

static void init ()
{
  /*init communication channel first*/
  spi_init ();

  /* init counter after spi */
  counter_init ();

  /* invoke needs a counter, so init it now*/
  invoke_init ();

  at_init ();
  buzz_init ();
  clock_init ();
  decode_init ();
  flush_init ();
  heartbeat_init ();
  mode_init ();
  postpone_init ();
  rotor_init ();
  /* ! init menu after rotor */
  menu_init ();
  power_init ();

  mode_set (MODE_CLOCK);

  sei ();
}

int main ()
{
  init ();

  while (1) {
    /* buzz_try (); */
    decode_try ();
    invoke_try ();
    postpone_try ();
    rotor_try ();
  }

  return 0;
}
