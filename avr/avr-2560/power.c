/*
 *
 */

#include <avr/io.h>

#include "power.h"

void power_init ()
{
  /*configure C0 as output */
  DDRC |= (1 << DDC0);

  power_up ();
}

void power_up ()
{
  /* assign 0 to switch on */
  PORTC &= ~(1 << PC0);
}

void power_down ()
{
  /* see power_up */
  PORTC |= (1 << PC0);
}
