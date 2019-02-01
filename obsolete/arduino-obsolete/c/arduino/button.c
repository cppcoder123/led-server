/*
 * Try to detect if one or more buttons is pressed.
 * 1. After a long delay put needed ports from low to high
 * 2. Then do a set of short delays and detect voltage rising speed
 *    if speed is low then button is pressed, else not
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#include "device-id.h"

#include "button.h"
#include "codec.h"

#define LONG_DELAY 1
#define SHORT_DELAY 250
#define RISE_THRESHOLD 120      /* fixme: find right value */

/* fixme: This value can be reduced probably  */
#define SHORT_DELAY_NUMBER 255

#define BUTTON_NUMBER 3

#define OUT_0 (1 << 0)
#define OUT_1 (1 << 2)
#define OUT_2 (1 << 4)
#define OUT_MASK (OUT_0 | OUT_1 | OUT_2)

#define IN_0 (1 << 1)
#define IN_1 (1 << 3)
#define IN_2 (1 << 5)

static volatile uint8_t button_delay_count[BUTTON_NUMBER];
static volatile uint8_t delay_count;

static volatile uint8_t old_button_pressed;
static volatile uint8_t send_disabled;

static void count_clear ()
{
  for (uint8_t i = 0; i < BUTTON_NUMBER; ++i)
    button_delay_count[i] = 0;

  delay_count = 0;

  old_button_pressed = 0;
  send_disabled = 0;
}

void button_init ()
{
  count_clear ();

  /*
   * All defaults here
   */
  TCCR2A = 0;

  /*
   * 1024 prescaler, 64 mcsec per tick
   */
  TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);

  /*
   * Enable overflow interrupt, always waiting
   */
  TIMSK2 = (1 << TOIE2);

  /* Hope this one is correct*/
  /* MCUCR = ? */

  /*configure: input is low, output is high*/
  DDRC = OUT_0 | OUT_1 | OUT_2;

  /* do we need a pullup ?*/
}

static void pin_check (uint8_t pin, volatile uint8_t *count)
{
  if ((PINC & pin) == 0)
    // pin didn't change its value, increment
    ++(*count);
}

static uint8_t button_count (uint8_t button_pushed)
{
  /*less than 8 buttons*/
  uint8_t one_button = ID_BUTTON_MIN;
  uint8_t sum = 0;
  while (one_button != ID_BUTTON_MAX) {
    if ((button_pushed & one_button) != 0)
      ++sum;
    one_button <<= 1;
  }

  return sum;
}

static void button_compare (uint8_t new_button_pressed)
{
  uint8_t old_count = button_count (old_button_pressed);
  uint8_t new_count = button_count (new_button_pressed);

  /* somebody is releasing buttons */
  if (new_count < old_count) {
    /*
     * uart write implemented using interrupts,
     * so button message should breakthrough event if matrix
     * is consumed 'main thread'
     */
    if (send_disabled == 0) {
      codec_encode_1
        (ID_BUTTON, ID_DEVICE_SERIAL, new_button_pressed);
      send_disabled = 1;
    }
    if (new_count == 0) {
      send_disabled = 0;
    }
  }
}

static void analyse_delays ()
{
  uint8_t button_pressed = 0;
  if (button_delay_count[0] > RISE_THRESHOLD)
    button_pressed |= ID_BUTTON_0;
  if (button_delay_count[1] > RISE_THRESHOLD)
    button_pressed |= ID_BUTTON_1;
  if (button_delay_count[2] > RISE_THRESHOLD)
    button_pressed |= ID_BUTTON_2;

  button_compare (button_pressed);

  old_button_pressed = button_pressed;
}

ISR (TIMER2_OVF_vect)
{
  if (delay_count == 0) {
    ++delay_count;
    /* set outputs to 1 */
    PORTC |= OUT_MASK;
    return;
  }

  ++delay_count;

  pin_check (IN_0, button_delay_count);
  pin_check (IN_1, button_delay_count + 1);
  pin_check (IN_2, button_delay_count + 2);

  if (delay_count < SHORT_DELAY_NUMBER) {
    TCNT2 = SHORT_DELAY;
    return;
  }

  /* enough short delays, analysis is required */
  analyse_delays ();
  count_clear ();
  /* set outputs to 0 */
  PORTC &= ~OUT_MASK;
  TCNT2 = LONG_DELAY;
}
