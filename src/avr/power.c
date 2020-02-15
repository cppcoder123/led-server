/*
 *
 */

#include <avr/io.h>

#include "at.h"
#include "power.h"

#define CONTROL_WIRE (1 << PORTE0)

#define OFF_DELAY 10
#define HOLD_DELAY 5

void power_init ()
{
  /* fixme: Configure power wire as output and set it to 0 */

  /* e0 => output  */
  DDRE |= CONTROL_WIRE;

  /* set it to 0 */
  PORTE &= ~CONTROL_WIRE;
}

static void raise_voltage ()
{
  PORTE &= CONTROL_WIRE;
}

static void drop_voltage ()
{
  PORTE |= CONTROL_WIRE;

  at_schedule (AT_POWER, HOLD_DELAY, &raise_voltage);
}

void power_on ()
{
  at_schedule (AT_POWER, OFF_DELAY, &drop_voltage);
}
