/*
 *
 */

#include <stdint.h>

#include "boost.h"
#include "fan.h"

static uint8_t started = 0;

void fan_init ()
{
  /* fixme */
  started = 0;
}

void fan_try ()
{
  /* fixme */
  if (started == 0)
    return;

  boost_try ();
}

void fan_start ()
{
  started = 1;
  boost_start ();
  /* fixme : init everything*/
}

void fan_stop ()
{
  /* fixme : de-init everything*/
  boost_stop ();
  started = 0;
}


