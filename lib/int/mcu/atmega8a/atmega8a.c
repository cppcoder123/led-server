/*
 * Handle two ht1632c controllers with led-matrices attached
 * Receive info by SPI interface
 */

#include <avr/sleep.h>

#include <stdint.h>
#include <stdlib.h>


#define F_CPU 16000000UL

/*state bits*/
#define STATE_IDLE          (1 << 0)
#define STATE_DATA_ARRIVED  (1 << 1)     /* data has arrived */
#define STATE_DATA_READY    (1 << 2)     /* new data is ready */
#define STATE_BRIGHTNESS    (1 << 3)

// max data array size
#define DATA_CAPACITY 128       /* fixme: can we increase this value? */

// display size
#define DISPLAY_HALF_CAPACITY 32
#define DISPLAY_CAPACITY  (DISPLAY_HALF_CAPACITY * 2)

//
volatile uint8_t state_flag = 0;
//
volatile uint8_t *led_data = 0;
volatile uint16_t led_data_size = 0;
//
volatile uint8_t *led_data_buffer = 0;
volatile uint16_t led_data_buffer_size = 0;
//
volatile uint8_t led_brightness = 0;
//
volatile uint8_t led_scroll_shift_delay = 0;

void initialize ()
{
  state_flag = STATE_IDLE;
  //
  led_data = (uint8_t*) calloc (DATA_CAPACITY, sizeof (uint8_t));
  led_data_buffer = (uint8_t*) calloc (DATA_CAPACITY, sizeof (uint8_t));
  //
  led_data_size = led_data_buffer_size = 0;
  //
  led_brightness = 16;
  //
  led_scroll_shift_delay = 10;        /* fixme : ? */
}

void idle_delay ()
{
  /*fixme : sleep a lot*/
}

void change_brightness ()
{
  /* fixme: change brigtness*/
}

void handle_incoming ()
{
  if (state_flag & STATE_DATA_ARRIVED) {
    for (uint16_t i = 0; i < led_data_buffer_size; ++i)
      led_data[i] = led_data_buffer[i];
    led_data_size = led_data_buffer_size;
    //
    state_flag &= ~STATE_DATA_ARRIVED;
    state_flag |= STATE_DATA_READY;
  }
  if (state_flag & STATE_BRIGHTNESS) {
    change_brightness ();
    state_flag &= ~STATE_BRIGHTNESS;
  }
}

void display_data (uint8_t index, uint8_t left_data, uint8_t right_data)
{
  // fixme
}

void clear ()
{
  // fixme: clear display
}

uint8_t get_data (uint16_t index)
{
  if (index >= led_data_size)
    return 0;

  return led_data[index];
}

void handle_data_fixed ()
{
  clear ();
  //
  for (uint8_t index = 0; index < DISPLAY_HALF_CAPACITY; ++index) 
    display_data (index,
                  get_data (index), get_data (index + DISPLAY_HALF_CAPACITY));

  // don't refresh static info
  state_flag &= ~STATE_DATA_READY;
}

void scroll_shift_delay ()
{
  // fixme: sleep time should depend on 'led_scroll_shift_delay'
}

void handle_data_scroll ()
{
  clear ();
  
  // imagine DISPLAY_CAPACITY columns before and after data
  uint16_t scroll_max_index = DISPLAY_CAPACITY + led_data_size;

  for (uint16_t scroll_index = 0; scroll_index < scroll_max_index; ++scroll_index) {
    for (uint8_t display_index = 0;
         display_index < DISPLAY_HALF_CAPACITY; ++display_index) {
      uint16_t column_index = scroll_index + display_index;
      uint16_t left_data_index = (column_index >= DISPLAY_CAPACITY)
        ? column_index - DISPLAY_CAPACITY : DATA_CAPACITY;
      uint16_t right_data_index = (column_index + DISPLAY_HALF_CAPACITY >= DISPLAY_CAPACITY)
        ? column_index + DISPLAY_HALF_CAPACITY - DISPLAY_CAPACITY : DATA_CAPACITY;
      display_data (display_index,
                    get_data (left_data_index), get_data (right_data_index));
    }
    scroll_shift_delay ();
  }

  // do not remove 'DATA_READY' flag, we want to launch scroll again
}

void handle_outcoming ()
{
  if (state_flag & STATE_DATA_READY) {
    if (led_data_size <= DISPLAY_CAPACITY)
      handle_data_fixed ();
    else
      handle_data_scroll ();
  }
}

int main ()
{
  // fixme: add hardware init here
  
  initialize ();
  
  while (1) {
    if (state_flag & STATE_IDLE) {
      idle_delay ();
      continue;
    }
    //
    handle_incoming ();              /* handle incoming spi info */
    //
    handle_outcoming ();             /* push info to led controllers */
  }
  
  return 0;
}
