/*
 *
 */

#include <stdint.h>

#include "device-id.h"

#include "button.h"
#include "codec.h"

#define BUTTON_SEND_DISABLED (ID_BUTTON_MAX << 1)

volatile uint8_t button_pressed;

void button_init ()
{
  button_pressed = 0;
  /* fixme */
}

static uint8_t button_detect_push ()
{
  /* fixme */
  return 0;
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
  uint8_t old_count = button_count (button_pressed);
  uint8_t new_count = button_count (new_button_pressed);

  /* somebody is releasing buttons */
  if (new_count < old_count) {
    /*
     * uart write implemented using interrupts,
     * so button message should breakthrough event if matrix
     * is consumed 'main thread'
     */
    if ((button_pressed & BUTTON_SEND_DISABLED) == 0) {
      codec_encode_1
        (ID_BUTTON, ID_DEVICE_SERIAL, button_pressed & ID_BUTTON_MASK);
      button_pressed |= BUTTON_SEND_DISABLED;
    }
    if (new_count == 0) {
      button_pressed &= (~BUTTON_SEND_DISABLED);
    }
  }
}

