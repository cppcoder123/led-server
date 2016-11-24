//
// Handle two ht1632c controllers with led-matrices attached
// Receive info by SPI interface
//

// it should be before includes
#define F_CPU 16000000UL

#include <util/delay.h>

#include <avr/interrupt.h>

#include <stdint.h>
#include <stdlib.h>

#include "libled/spi-message.h"


// transfer state flags
#define TRANSFER_IDLE            (1 << 0)
#define TRANSFER_READING         (1 << 1) // else writing
#define TRANSFER_MATRIX_STARTED  (1 << 2)
#define TRANSFER_MATRIX_RENDERED (1 << 3)

// max matrix size
#define MATRIX_CAPACITY SPI_MATRIX_CAPACITY

// display size
#define DISPLAY_HALF_CAPACITY 32
#define DISPLAY_CAPACITY (DISPLAY_HALF_CAPACITY * 2)

// timer0 clock factor
#define TIMER0_FACTOR 15

//
volatile uint8_t transfer_flag = 0;
//
//
volatile uint8_t *transfer_buffer = 0;
volatile uint8_t transfer_buffer_size = 0;
volatile uint16_t transfer_index = 0;
//
volatile uint8_t *matrix = 0;
volatile uint16_t matrix_size = 0;
//
volatile uint8_t brightness = 0;
//
volatile uint8_t scroll_shift_delay = 0;
//
//

//
// Handle incoming messages
//

// message parse is complete
void spi_read_complete (uint8_t status)
{
  // ignore spi
  transfer_flag |= TRANSFER_IDLE;
  // fill reply buffer
  transfer_buffer[0] = SPI_MASTER_START;
  transfer_buffer[1] = status;
  transfer_buffer[3] = SPI_MASTER_FINISH;
  transfer_buffer_size = 3;
  //
  SPDR = transfer_buffer[0];
  transfer_index = 0;

  // start writing
  transfer_flag &= ~(TRANSFER_IDLE | TRANSFER_READING);
}
 
void spi_read_start (uint8_t info)
{
  if (transfer_buffer_size++ == 2) {
    // fixme : init ht1632c here
    spi_read_complete
      ((info == SPI_SLAVE_FINISH) ? SPI_STATUS_OK : SPI_STATUS_NO_FINISH);
  }
}

void spi_read_stop (uint8_t info)
{
  if (transfer_buffer_size++ == 2) {
    // fixme : stop ht1632c here
    spi_read_complete
      ((info == SPI_SLAVE_FINISH) ? SPI_STATUS_OK : SPI_STATUS_NO_FINISH);
  }
}

void spi_read_handshake (uint8_t info)
{
  if (transfer_buffer_size++ == 2) {
    // just send reply
    spi_read_complete
      ((info == SPI_SLAVE_FINISH) ? SPI_STATUS_OK : SPI_STATUS_NO_FINISH);
  }
}

void spi_read_matrix (uint8_t info)
{
  transfer_flag |= TRANSFER_MATRIX_STARTED;
  switch (transfer_buffer_size) {
  case 2:
    matrix_size = info;
    ++transfer_buffer_size;
    break;
  case 3:
    matrix_size += 255 * info;
    ++transfer_buffer_size;
    break;
  case 4:
    if (info != SPI_MATRIX_ARRAY_START) {
      spi_read_complete (SPI_STATUS_NO_ARRAY_START);
    } else {
      transfer_index = 0;
      ++transfer_buffer_size;
    }
    break;
  default:
    if (transfer_index < matrix_size) {
      matrix[transfer_index++] = info;
    } else {
      spi_read_complete
        ((info == SPI_SLAVE_FINISH)
         ? SPI_STATUS_OK : SPI_STATUS_NO_FINISH);
      transfer_flag &= ~TRANSFER_MATRIX_STARTED;
    }
  }
}

void spi_read_delay (uint8_t info)
{
  switch (transfer_buffer_size++) {
  case 2:
    if (info != SPI_DELAY_SCROLL_SHIFT)
      spi_read_complete (SPI_STATUS_UNKNOWN_DELAY_ID);
    break;
  case 3:
    scroll_shift_delay = info;
    break;
  default:
    spi_read_complete
      ((info == SPI_SLAVE_FINISH)
       ? SPI_STATUS_OK : SPI_STATUS_NO_FINISH);
      break;
  }
}

void spi_read_brightness (uint8_t info)
{
  switch (transfer_buffer_size++) {
  case 2:
    transfer_buffer[2] = info;
    break;
  default:
    if ((transfer_buffer[2] < SPI_BRIGHTNESS_MIN)
        || (transfer_buffer[2] > SPI_BRIGHTNESS_MAX))
      spi_read_complete (SPI_STATUS_BRIGHTNESS_OUT_OF_RANGE);
    brightness = transfer_buffer[2];
    // fixme : send brightness to ht1632c here
    spi_read_complete
      ((info == SPI_SLAVE_FINISH)
       ? SPI_STATUS_OK : SPI_STATUS_NO_FINISH);
    break;
  }
}

void switch_relay (uint8_t state)
{
  if (state != 0)
    PORTD |= (1 << PD7);
  else
    PORTD &= ~(1 << PD7);
}

void spi_read_switch_relay (uint8_t info)
{
  switch (transfer_buffer_size++) {
  case 2:
    transfer_buffer[2] = info;
    break;
  default:
    switch_relay (transfer_buffer[2]);
    spi_read_complete
      ((info == SPI_SLAVE_FINISH) ? SPI_STATUS_OK : SPI_STATUS_NO_FINISH);
    break;
  }
}

void spi_read (uint8_t info)
{
  if (transfer_buffer_size == 0) {
    if (info == SPI_SLAVE_START)
      transfer_buffer[transfer_buffer_size++] = SPI_SLAVE_START;
  } else if (transfer_buffer_size == 1) {
    // we expect msg id now
    if ((info < SPI_SLAVE_MSG_MIN)
        || (info >= SPI_SLAVE_MSG_MAX)) {
      spi_read_complete (SPI_STATUS_MESSAGE_ID_UNKNOWN);
    } else {
      transfer_buffer[transfer_buffer_size++] = info;
    }
  } else {
    switch (transfer_buffer[1]) {
    case SPI_SLAVE_MSG_START:
      spi_read_start (info);
      break;
    case SPI_SLAVE_MSG_STOP:
      spi_read_stop (info);
      break;
    case SPI_SLAVE_MSG_HANDSHAKE:
      spi_read_handshake (info);
      break;
    case SPI_SLAVE_MSG_MATRIX:
      spi_read_matrix (info);
      break;
    case SPI_SLAVE_MSG_DELAY:
      spi_read_delay (info);
      break;
    case SPI_SLAVE_MSG_BRIGHTNESS:
      spi_read_brightness (info);
      break;
    case SPI_SLAVE_MSG_SWITCH_RELAY:
      spi_read_switch_relay (info);
      break;
    default:
      // we shouldn't reach this point
      spi_read_complete (SPI_STATUS_LONG_MESSAGE_ID_UNKNOWN);
      break;
    }
  }
}

//
// Interrupt handling
//

ISR(SPI_STC_vect)
{
  if (transfer_flag & TRANSFER_IDLE)
    return;

  if (transfer_flag & TRANSFER_READING) {
    uint8_t info = SPDR;
    // don't send junk to master
    SPDR = 0;
    spi_read (info);
    //transfer_flag |= TRANSFER_BYTE_READY;
  } else {
    ++transfer_index;
    if (transfer_index < transfer_buffer_size)
      SPDR = transfer_buffer[transfer_index];
    else {
      // write completed => start reading
      transfer_flag |= TRANSFER_READING;
    }
  }
}

//
// Init code
//

void hw_init ()
{
  // Set pin directions
  
  // a. port D - 0-2 for debug - output, 7 - relay
  DDRD |= ((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD7));
  // b. port C - communicate with ht1632 - output
  //    3 wires per each ht1632
  DDRC |= ((1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5));
  // c. port B - SPI interface - slave - pb2,3,5 - input, pb4 - output
  // fixme: this code is not required, init by SPI logic
  DDRB |= (1 << PB4);
  DDRB &= ~((1 << PB2) | (1 << PB3) | (1 << PB5));

  // Configure SPI as slave

  // spi interrupt enable, spi enable, lsb first
  SPCR |= ((1 << SPIE) | (1 << SPE) | (1 << DORD));
  // the same set of (CPOL, CPHA) sould be used for master
  // slave, SCK low => idle, leading edge sampling (fixme?)
  SPCR &= ~((1 << MSTR) | (1 << CPOL) | (1 << CPHA));
  // clock rate selection is not required for slave

  // clear spi data
  SPDR = 0;

  // Enable inerrupts globally
  sei ();
  //
  //
}

void sw_init ()
{
  transfer_flag = TRANSFER_READING;
  //
  //
  transfer_buffer = (uint8_t*) calloc (SPI_SLAVE_MAX_MSG_LENGTH, sizeof (uint8_t));
  transfer_buffer_size = 0;
  //
  transfer_index = 0;
  //
  matrix = (uint8_t*) calloc (MATRIX_CAPACITY, sizeof (uint8_t));
  matrix_size = 0;
  //
  brightness = SPI_BRIGHTNESS_MAX;
  //
  scroll_shift_delay = 30;      /* fixme : ? */
}

//
// Handle led displays
//

uint8_t is_matrix_consistent ()
{
  return (transfer_flag & TRANSFER_MATRIX_STARTED) ? 0 : 1;
}

void busy_wait (uint8_t ms, uint8_t factor)
{
  for (uint8_t i = 0; i < factor; ++i) {
    for (uint8_t j = 0; j < ms; ++j) {
      _delay_ms (1);
        if (!is_matrix_consistent ())
          return;
    } // j
  } // i
}

void scroll_shift_sleep ()
{
  busy_wait (scroll_shift_delay, 10);
}

void scroll_cycle_sleep ()
{
  // fixme: do we need to sleep longer?
  // 10? times more than scroll_shift_sleep
  scroll_shift_sleep ();
}

void change_brightness ()
{
  /* fixme: change brigtness*/
}

void display_data (uint8_t index, uint8_t left_data, uint8_t right_data)
{
  // fixme
}

void clear ()
{
  for (uint8_t i = 0; i < DISPLAY_HALF_CAPACITY; i++)
    display_data (i, 0, 0);
}

uint8_t get_column (uint16_t index)
{
  if ((!is_matrix_consistent ())
      || (index >= matrix_size))
    return 0;
  
  return matrix[index];
}

void idle_sleep ()
{
  // sleep a lot (~1 sec)
  busy_wait (100, 10);
}

void render_matrix_fixed ()
{
  if (transfer_flag & TRANSFER_MATRIX_RENDERED) {
    idle_sleep ();
    return;
  }
  
  clear ();
  //
  for (uint8_t index = 0; index < DISPLAY_HALF_CAPACITY; ++index) {
    if (!is_matrix_consistent ())
      return;
    display_data (index,
                  get_column (index),
                  get_column (index + DISPLAY_HALF_CAPACITY));
  }

  // don't refresh static info
  transfer_flag |= TRANSFER_MATRIX_RENDERED;
}

void render_matrix_scroll ()
{
  clear ();
  
  // imagine DISPLAY_CAPACITY columns before and after data
  uint16_t scroll_max = DISPLAY_CAPACITY + matrix_size;

  for (uint16_t scroll = 0; scroll < scroll_max; ++scroll) {
    for (uint8_t display = 0; display < DISPLAY_HALF_CAPACITY; ++display) {
      if (!is_matrix_consistent ())
        return;
      uint16_t column = scroll + display;
      uint16_t left_index = (column >= DISPLAY_CAPACITY)
        ? column - DISPLAY_CAPACITY : MATRIX_CAPACITY;
      uint16_t right_index = (column + DISPLAY_HALF_CAPACITY >= DISPLAY_CAPACITY)
        ? column + DISPLAY_HALF_CAPACITY - DISPLAY_CAPACITY : MATRIX_CAPACITY;
      display_data (display, get_column (left_index), get_column (right_index));
    }
    scroll_shift_sleep ();
  }
  scroll_cycle_sleep ();

  // do not remove 'MATRIX_RENDERED' flag, we want to launch scroll again
}

//
// Test debugging leds
//
void debug_sleep ()
{
  // it should be equal ~0.5 sec
  busy_wait (50, 10);
  
}

void debug_test_led (uint8_t leg, uint8_t on)
{
  if (on) {
    PORTD |= (1 << leg);
    debug_sleep ();
  } else {
    PORTD &= ~(1 << leg);
    debug_sleep ();
  }
}

void debug_test ()
{
  // test miso voltage level
  PORTB |= (1 << PB4);
  
  // test relay
  switch_relay (1);
  
  PORTD &= ~((1 << PD0) | (1 << PD1) | (1 << PD2));

  debug_sleep ();

  for (uint8_t i = 0; i < 10; i++) {
    //
    debug_test_led (PD0, 1);
    debug_test_led (PD1, 1);
    debug_test_led (PD2, 1);
    //
    debug_test_led (PD0, 0);
    debug_test_led (PD1, 0);
    debug_test_led (PD2, 0);
    //
  }

  // turn relay off
  switch_relay (0);

  // turn miso off
  PORTB &= ~(1 << PB4);

}

//
// Main
//
int main ()
{
  hw_init ();
  
  sw_init ();

  debug_test ();
  
  while (1) {
    //
    if (is_matrix_consistent ()) {
      if (matrix_size <= DISPLAY_CAPACITY)
        render_matrix_fixed ();
      else
        render_matrix_scroll ();
    }
  }
  
  return 0;
}
