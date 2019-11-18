/*
 *
 */

#include <avr/interrupt.h>

//#include "buzz.h"
#include "decode.h"
#include "debug.h"
#include "display.h"
#include "flush.h"
/* #include "key-board.h" */
#include "postpone.h"
//#include "power.h"
#include "spi.h"
/* #include "twi.h" */

static void init ()
{
  /*init channel first*/
  spi_init ();

  /* buzz_init (); */
  /* debug_init (); */
  decode_init ();
  display_init ();
  flush_init ();
  /* key_board_init (); */
  postpone_init ();
  /* power_init (); */
  /* twi_init (); */

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
    /* key_board_try (); */
    postpone_try ();
    /* twi_try (); */
  }

  return 0;
}
