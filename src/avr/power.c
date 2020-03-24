/*
 *
 */

#include <avr/io.h>

#include "at.h"
#include "power.h"

#define CONTROL_WIRE (1 << PORTJ0)

static void disconnect_control_wire ()
{
  PORTJ &= ~CONTROL_WIRE;
}

void power_init ()
{
  /* J0 => output  */
  DDRJ |= CONTROL_WIRE;

  /* disconnect control wire, make it high-z */
  disconnect_control_wire ();
}


void power_on ()
{
  /* short control wire to ground and wait sometime */
  PORTJ |= CONTROL_WIRE;

  at_schedule (AT_POWER, 2, &disconnect_control_wire);
}
