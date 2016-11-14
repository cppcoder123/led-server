//
// Handle two ht1632c controllers with led-matrices attached
// Receive info by SPI interface
//

// it should be before includes
#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include <stdint.h>
#include <stdlib.h>

#include "libled/spi-message.h"


// transfer state flags
#define TRANSFER_IDLE    (1 << 0)
#define TRANSFER_READING (1 << 1) // else writing
#define TRANSFER_READY   (1 << 2) // byte has arrived or sent
#define TRANSFER_MATRIX_STARTED  (1 << 3)
#define TRANSFER_MATRIX_RENDERED (1 << 4)

// parser state flags
//#define STATE_SPI_READING               (1 << 1)
//#define STATE_SPI_WRITING               (1 << 2)
//#define STATE_SPI_BYTE_READ             (1 << 3)
//#define STATE_MATRIX_TRANSFER_STARTED   (1 << 4)
//#define STATE_MATRIX_RENDERED           (1 << 5)
//#define STATE_BRIGHTNESS                (1 << 6)


// max matrix size
#define MATRIX_CAPACITY SPI_MATRIX_CAPACITY

// display size
#define DISPLAY_HALF_CAPACITY 32
#define DISPLAY_CAPACITY (DISPLAY_HALF_CAPACITY * 2)

//
volatile uint8_t transfer_flag = 0;
//
volatile uint8_t transfer_byte = 0;
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
//volatile uint8_t parse_message_id = 0;
//volatile uint16_t parse_step = 0;
//volatile uint8_t parse_byte = 0;

//
// Interrupt handling
//

ISR(SPI_STC_vect)
{
  if (transfer_flag & TRANSFER_IDLE)
    return;

  if (transfer_flag & TRANSFER_READING) {
    transfer_byte = SPDR;
    // don't send junk to master
    SPDR = 0;
    transfer_flag |= TRANSFER_READY;
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

/*   SPDR = transfer_buffer[transfer_index]; */
/*   if (transfer_index++ >) */
/*   transfer_index */
/*   SPDR = transfer_bubyte; */
/* } */

// ignore everything if we are not ready
//}
  
//  if (transfer_flag & TRANSFER_READING) {
//  } else {
//    // we are writing
//  }

  
//  if (state_flag & STATE_SPI_WRITING) {
//    state_flag &= ~STATE_SPI_WRITING;
//    // end of writing => we can read
//    state_flag |= STATE_SPI_READING;
//    // ingore end of send
//    return;
//  }
//
//  if (!(state_flag & STATE_SPI_READING))
//    return;

//  if (!(state_flag & STATE_SPI_EYE_CATCHED)) {
//  if (SPDR == SPI_SLAVE_EYE_CATCH) {
//    state_flag &= ~STATE_SPI_READING;
//    state_flag |= STATE_SPI_EYE_CATCHED;
//  }
//  // ignore everything except eye-catch
//  return;
//}

// we want to receive something
//  parse_byte = SPDR;
// // ignore everything while parsing byte
//  state_flag &= ~STATE_SPI_READING;
//  state_flag |= STATE_SPI_BYTE_READ;
//}

//
// Handle incoming messages
//

// message parse is complete
void spi_parse_complete (uint8_t status)
{
  // ignore spi
  transfer_flag |= TRANSFER_IDLE;
  // fill reply buffer
  transfer_buffer[0] = SPI_MESSAGE_MASTER_START;
  transfer_buffer[1] = status;
  transfer_buffer[3] = SPI_MESSAGE_MASTER_FINISH;
  transfer_buffer_size = 3;
  //
  SPDR = transfer_buffer[0];
  transfer_index = 0;

  //
  transfer_flag &= ~(TRANSFER_IDLE | TRANSFER_READING);
}
 
/*   transfer_byte = status; */
/*   transfer_buffer_size = 0; */
/*   // start writing */
/*   transfer_flag &= ~TRANSFER_READING; */
/* } */
  
/*   parse_message_id = SPI_MESSAGE_EMPTY; */
/*   parse_step = 0; */
/*   state_flag &= ~STATE_SPI_EYE_CATCHED; */
/* } */

void spi_start ()
{
  // fixme
  spi_parse_complete (SPI_STATUS_OK);
}

void spi_stop ()
{
  // fixme
  spi_parse_complete (SPI_STATUS_OK);
}

void spi_handshake ()
{
  // we can reach this point only with 'SPI_MESSAGE_SLAVE_FINISH'
  if (transfer_byte == SPI_MESSAGE_SLAVE_FINISH)
    spi_parse_complete (SPI_STATUS_OK);
  else
    spi_parse_complete (SPI_STATUS_NO_FINISH);
}

void spi_matrix ()
{
  transfer_flag |= TRANSFER_MATRIX_STARTED;
  switch (transfer_buffer_size) {
  case 2:
    matrix_size = transfer_byte;
    ++transfer_buffer_size;
    break;
  case 3:
    matrix_size += 255 * transfer_byte;
    ++transfer_buffer_size;
    break;
  case 4:
    if (transfer_byte != SPI_MATRIX_ARRAY_START) {
      spi_parse_complete (SPI_STATUS_NO_ARRAY_START);
    } else {
      transfer_index = 0;
      ++transfer_buffer_size;
    }
    break;
  default:
    if (transfer_index < matrix_size) {
      matrix[transfer_index++] = transfer_byte;
    } else if (transfer_index == matrix_size) {
      if (transfer_byte == SPI_MESSAGE_SLAVE_FINISH)
        spi_parse_complete (SPI_STATUS_OK);
      else
        spi_parse_complete (SPI_STATUS_NO_FINISH);
      transfer_flag &= TRANSFER_MATRIX_STARTED;
    } else {
      spi_parse_complete (SPI_STATUS_PARSE_ERROR);
      transfer_flag &= TRANSFER_MATRIX_STARTED;
    }
  }
}

    
/*     index = parse_step - 5; */
/*     if (index > SPI_MATRIX_CAPACITY) */
/*       return SPI_STATUS_TOO_LONG_MATRIX; */
/*     if (index == matrix_size) { */
/*       if (data != SPI_MATRIX_ARRAY_FINISH) */
/*         return SPI_STATUS_NO_ARRAY_FINISH; */
/*       // message receive is completed */
/*       state_flag &= ~STATE_MATRIX_TRANSFER_STARTED; */
/*       parse_complete (); */
/*     } else { */
/*       matrix[index] = data; */
/*     } */
/*     break; */
/*   } */

/*   return SPI_STATUS_OK; */
/* } */

void spi_delay ()
{
  switch (transfer_buffer_size++) {
  case 2:
    if (transfer_byte != SPI_DELAY_SCROLL_SHIFT)
      spi_parse_complete (SPI_STATUS_UNKNOWN_DELAY_ID);
    break;
  case 3:
    scroll_shift_delay = transfer_byte;
    break;
  case 4:
    spi_parse_complete
      ((transfer_byte == SPI_MESSAGE_SLAVE_FINISH)
       ? SPI_STATUS_OK : SPI_STATUS_NO_FINISH);
      break;
  default:
    spi_parse_complete (SPI_STATUS_PARSE_ERROR);
    break;
  }
}
/*   parse_step++; */
/*   if (parse_step == 2) { */
/*     if (data != SPI_DELAY_SCROLL_SHIFT) */
/*       return SPI_STATUS_UNKNOWN_DELAY_ID; */
/*   } else if (parse_step == 3) { */
/*     scroll_shift_delay = data; */
/*     parse_complete (); */
/*   } */

/*   return SPI_STATUS_OK; */
/* } */

void spi_brightness ()
{
  switch (transfer_buffer_size++) {
  case 2:
    transfer_buffer[transfer_buffer_size] = transfer_byte;
    break;
  case 3:
    if ((transfer_buffer[2] < SPI_BRIGHTNESS_MIN)
        || (transfer_buffer[2] > SPI_BRIGHTNESS_MAX))
      spi_parse_complete (SPI_STATUS_BRIGHTNESS_OUT_OF_RANGE);
    brightness = transfer_buffer[2];
    // fixme : send brightness to ht1632c here
    if (transfer_byte == SPI_MESSAGE_SLAVE_FINISH)
      spi_parse_complete (SPI_STATUS_OK);
    else
      spi_parse_complete (SPI_STATUS_NO_FINISH);
    break;
  default:
    spi_parse_complete (SPI_STATUS_PARSE_ERROR);
    break;
  }
}
/*   if ((transfer_byte < SPI_BRIGHTNESS_MIN) */
/*       || (transfer_byte > SPI_BRIGHTNESS_MAX)) */
/*     spi_parse_complete (SPI_STATUS_BRIGHTNESS_OUT_OF_RANGE); */

/*   spi_parse_complete (SPI_STATUS_OK); */


  
/*   switch (transfer_buffer_size++) { */
/*   case 2: */
/*     break */
/*   } */
  
/*   // 2 steps parse, so data should be brightness */
/*   if ((data < SPI_BRIGHTNESS_MIN) */
/*       || (data > SPI_BRIGHTNESS_MAX)) */
/*     return SPI_STATUS_BRIGHTNESS_OUT_OF_RANGE; */

/*   brightness = data; */
/*   state_flag |= STATE_BRIGHTNESS; */
  
/*   parse_complete (); */

/*   return SPI_STATUS_OK; */
/* } */

void spi_read ()
{
  if (transfer_buffer_size == 0) {
    if (transfer_byte == SPI_MESSAGE_SLAVE_START)
      transfer_buffer[transfer_buffer_size++] = SPI_MESSAGE_SLAVE_START;
  } else if (transfer_buffer_size == 1) {
    // we expect msg id now
    if ((transfer_byte < SPI_MESSAGE_MIN)
        || (transfer_byte >= SPI_MESSAGE_MAX)) {
      spi_parse_complete (SPI_STATUS_MESSAGE_ID_UNKNOWN);
    } else {
      transfer_buffer[transfer_buffer_size++] = transfer_byte;
    }
  } else {
    switch (transfer_buffer[1]) {
    case SPI_MESSAGE_START:
      spi_start ();
      break;
    case SPI_MESSAGE_STOP:
      spi_stop ();
      break;
    case SPI_MESSAGE_HANDSHAKE:
      spi_handshake ();
      break;
    case SPI_MESSAGE_MATRIX:
      spi_matrix ();
      break;
    case SPI_MESSAGE_DELAY:
      spi_delay ();
      break;
    case SPI_MESSAGE_BRIGHTNESS:
      spi_brightness ();
      break;
    default:
      // we shouldn't reach this point
      spi_parse_complete (SPI_STATUS_LONG_MESSAGE_ID_UNKNOWN);
      break;
    }
  }
}

/*   uint8_t status = SPI_STATUS_OK; */
/*   if (parse_message_id == SPI_MESSAGE_EMPTY) { */
/*     if ((data >= SPI_MESSAGE_MIN) */
/*         && (data < SPI_MESSAGE_MAX)) { */
/*       // handle one byte messages right here */
/*       switch (data) { */
/*       case SPI_MESSAGE_START: */
/*         status = handle_start (); */
/*         break; */
/*       case SPI_MESSAGE_STOP: */
/*         status = handle_stop (); */
/*         break; */
/*       case SPI_MESSAGE_HANDSHAKE: */
/*         // status initialized with OK */
/*         break; */
/*       default: */
/*         parse_message_id = data; */
/*         parse_step = 1; */
/*         break; */
/*       } */
/*     } else { */
/*       status = SPI_STATUS_MESSAGE_ID_UNKNOWN; */
/*     } */

/*     parse_complete (); */
/*     return status; */
/*   } */

/*   // not a first step in message parsing */
/*   switch (parse_message_id) { */
/*   case SPI_MESSAGE_MATRIX: */
/*     status = parse_matrix (data); */
/*     break; */
/*   case SPI_MESSAGE_DELAY: */
/*     status = parse_delay (data); */
/*     break; */
/*   case SPI_MESSAGE_BRIGHTNESS: */
/*     status = parse_brightness (data); */
/*     break; */
/*   default: */
/*     // ! we should not reach this point */
/*     status = SPI_STATUS_LONG_MESSAGE_ID_UNKNOWN; */
/*     break; */
/*   } */

/*   return status; */
/* } */

/* void spi_write (uint8_t data) */
/* { */
/*   state_flag &= ~STATE_SPI_BYTE_READ; */
/*   state_flag |= STATE_SPI_WRITING; */
/*   SPDR = data; */
/* } */

//
// Init code
//

void hw_init ()
{
  // Set pin directions
  
  // a. port D - 0-4 for debug - output
  DDRD |= (1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3) | (1 << PD4);
  // b. port C - communicate with ht1632 - output
  //    3 wires per each ht1632
  DDRC |= (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5);
  // c. port B - SPI interface - slave - pb2,3,5 - input, pb4 - output
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
  transfer_flag = 0;
  //
  transfer_byte = 0;
  //
  transfer_buffer = (uint8_t*) calloc (SPI_MESSAGE_MAX_LENGTH, sizeof (uint8_t));
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

void scroll_shift_sleep ()
{
  // fixme: sleep time should depend on 'led_scroll_shift_delay'
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

uint8_t is_matrix_consistent ()
{
  return (transfer_flag & TRANSFER_MATRIX_STARTED) ? 0 : 1;
}

uint8_t get_column (uint16_t index)
{
  if ((!is_matrix_consistent ())
      || (index >= matrix_size))
    return 0;
  
  return matrix[index];
}

void render_matrix_fixed ()
{
  if (transfer_flag & TRANSFER_MATRIX_RENDERED)
    return;
  
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
  _delay_ms (500);
}

void debug_test_led (uint8_t leg)
{
  PORTD |= (1 << leg);
  debug_sleep ();
  PORTD &= ~(1 << leg);
  debug_sleep ();
}

void debug_test ()
{
  PORTD &= ~((1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3) | (1 << PD4));

  debug_sleep ();
  
  for (uint8_t i = 0; i < 5; i++) {
    debug_test_led (PD0);
    debug_test_led (PD1);
    debug_test_led (PD2);
    debug_test_led (PD3);
    debug_test_led (PD4);
  }
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
    //if (state_flag & STATE_SPI_EYE_CATCHED) {
    //  spi_write (SPI_STATUS_OK);
    //}
    //    if (state_flag & STATE_SPI_BYTE_READ) {
    //      spi_write (spi_read (parse_byte));
    //      state_flag &= ~STATE_SPI_BYTE_READ;
    //    }
    //
    //    if (state_flag & STATE_BRIGHTNESS) {
    //      change_brightness ();
    //      state_flag &= ~STATE_BRIGHTNESS;
    //    }
    //
    if (transfer_flag & TRANSFER_READY) {
      if (transfer_flag & TRANSFER_READING) {
        spi_read ();
        transfer_flag &= ~TRANSFER_READY;
      } 
    }
    //
    if (is_matrix_consistent ()) {
      if (matrix_size < DISPLAY_CAPACITY)
        render_matrix_fixed ();
      else
        render_matrix_scroll ();
    }
  }
  
  return 0;
}
