/*
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "at.h"
#include "buzz.h"
#include "counter.h"
#include "cron.h"
#include "decode.h"
#include "flush.h"
#include "heartbeat.h"
#include "menu.h"
#include "mode.h"
#include "postpone.h"
#include "rotor.h"
#include "spi.h"
#include "twi.h"
#include "watch.h"

static void decelerate ()
{
  /* enable clock change */
  CLKPR |= (1 << CLKPCE);

  /* set new clock prescaler => divide clock speed by 4*/
  /* CLKPR |= (1 << CLKPS1); */
  CLKPR |= (1 << CLKPS3);
  /* CLKPR |= (1 << CLKPS0) | (1 << CLKPS0); */
}

static void init ()
{
  /*init communication channel first*/
  spi_init ();

  /* init counter after spi */
  counter_init ();

  /* cron needs a counter, so init it now*/
  cron_init ();

  at_init ();
  /* check buzz */
  buzz_init ();
  decode_init ();
  flush_init ();
  heartbeat_init ();
  mode_init ();
  postpone_init ();
  menu_init ();
  rotor_init ();
  twi_init ();
  /* ! dac after twi_init */
  /* dac_init (); */
  /* ! watch after twi */
  watch_init ();

  mode_set (MODE_WATCH);

  sei ();
}

int main ()
{
  /* reduce the speed, we don't need it */
  decelerate ();
  
  init ();

  while (1) {
    counter_try ();
    cron_try ();
    decode_try ();
    postpone_try ();
    rotor_try ();
    twi_try ();
  }

  return 0;
}
