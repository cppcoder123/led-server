/*
 *
 */

#include <avr/io.h>

#include "at.h"
#include "boot.h"

#define BOOT_PORT PORTB
#define BOOT_WIRE PORTB6
#define BOOT_DDR DDRB

/* hold line approx 2 seconds */
#define HOLD_TIME 2

static void release_wire ()
{
  /* configure as input and let it to go high due to pull up*/
  BOOT_DDR &= ~(1 << BOOT_WIRE);
}

void boot_pi ()
{
  /* 1. configure as output */
  BOOT_DDR |= (1 << BOOT_WIRE);

  /* 2. set 0 to wire to start pi boot */
  BOOT_PORT &= ~(1 << BOOT_WIRE);

  /* 3. release wire after some time */
  at_schedule (AT_BOOT, HOLD_TIME, &release_wire);
}
