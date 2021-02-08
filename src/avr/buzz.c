/*
 *
 */

#include <avr/io.h>

#include "at.h"
#include "buzz.h"
#include "counter.h"

#define BUZZ_COUNTER COUNTER_4
#define BUZZ_PRESCALER COUNTER_PRESCALER_8
/* pwm polarity */
#define BUZZ_POLARITY 1

#define BUZZ_PORT PORTH
#define BUZZ_BIT PORTH4
#define BUZZ_DDR DDRH

#define BUZZ_PITCH 255
#define BUZZ_VOLUME 127

#define BUZZ_ZERO 0

#define BUZZ_INIT_DURATION 5

void buzz_start ()
{
  counter_register_write
    (BUZZ_COUNTER, COUNTER_OUTPUT_COMPARE_A, BUZZ_PITCH, BUZZ_ZERO);
  counter_register_write
    (BUZZ_COUNTER, COUNTER_OUTPUT_COMPARE_B, BUZZ_VOLUME, BUZZ_ZERO);
  counter_pwm_enable (BUZZ_COUNTER, BUZZ_POLARITY);
  counter_enable (BUZZ_COUNTER, BUZZ_PRESCALER);
}

void buzz_stop ()
{
  counter_disable (BUZZ_COUNTER);
  counter_pwm_disable (BUZZ_COUNTER, BUZZ_POLARITY);

  /* set buzz bit to zero */
  BUZZ_PORT &= ~(1 << BUZZ_BIT);
}

void buzz_init ()
{
  /* configure buzz bit as output */
  BUZZ_DDR |= (1 << BUZZ_BIT);

  buzz_start ();
  at_schedule (AT_BUZZ, BUZZ_INIT_DURATION, &buzz_stop);
}
