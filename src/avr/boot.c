/*
 *
 */

#include <avr/io.h>

#include "at.h"
#include "boot.h"

#define LINE PORTB6

/* hold line approx 2 seconds */
#define LINE_HOLD 2

static void line_release ()
{
  PORTB |= (1 << LINE);
}

void boot_init ()
{
  /* B6 => output  */
  DDRB |= (1 << LINE);

  /* set line to high-z state */
  line_release ();
}


void boot_pi ()
{
  /* drive line to zero to initiate the boot and wait sometime */
  PORTB &= ~(1 << LINE);

  at_schedule (AT_BOOT, LINE_HOLD, &line_release);
}
