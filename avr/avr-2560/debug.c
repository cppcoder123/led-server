/*
 *
 */

#include <avr/io.h>

#include "debug.h"

#define LED_PORT PORTC6

void debug_init ()
{
  DDRC |= (1 << LED_PORT);
  /*check*/
  debug_switch (0);
}

void debug_switch (uint8_t state)
{
  if (state != 0)
    PORTC |= (1 << LED_PORT);
  else
    PORTC &= ~(1 << LED_PORT);
}
