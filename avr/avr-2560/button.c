/*
 *
 */
#include <avr/interrupt.h>
#include <stdint.h>

#include "constant.h"
#include "encode.h"
#include "button.h"

/*
 * Handling
 */

#define SIZE 8

/*fixme: adjust*/
#define VOLTAGE_LOW_THRESHOLD 110
#define VOLTAGE_HIGH_THRESHOLD 160

#define SRC_LOW(x) PORTA &= ~(1 << x)
#define SRC_HIGH(x) PORTA |= (1 << x)

#define MUX_MASK ((1 << MUX0) | (1 << MUX1) | (1 << MUX2))
#define SELECT_BUTTON(x) ADMUX &= ~MUX_MASK; ADMUX |= x

/*we have 0.1sec adc cycle, so aprox 0.7 sec is a threshold*/
#define TIME_THRESHOLD 7

/*
 * Timer
 */

/*CSn1 => 1/8 => timer clock 2Mhz*/
#define SHORT_PRESCALER (1 << CS51)

/*slowest clock to make a pause between measures*/
#define LONG_PRESCALER ((1 << CS50) | (1 << CS52))

/*tool to stop the timer*/
#define PRESCALER_MASK (SHORT_PRESCALER | (LONG_PRESCALER))

#define STOP_TIMER TCCR5B &= ~PRESCALER_MASK

/*
 * We want to measure middle of rising front,
 * human capacity ~100-200 pF and we have 1M
 * resistor, so (100*10^-12) * (1-^6) = 0.1 msec.
 * So half of rising time should be 0.05 msec = 50 usec.
 * With 2mHz clock we need 100 (or 200) cycles
 */
#define SHORT_DELAY 110

/*
 * We need aprox. 10 measures per second
 * 16 mHz / 1024 => 15625 kHz
 * 10 measures per second => 1562
 */
#define LONG_DELAY 1562

#define SLOW_MODE                               \
  OCR5A = LONG_DELAY;                           \
  TCCR5B |= LONG_PRESCALER;

#define FAST_MODE                               \
  OCR5A = SHORT_DELAY;                          \
  TCCR5B |= SHORT_PRESCALER;

enum {
  BUTTON_0,
  BUTTON_1,
  BUTTON_2,
  BUTTON_3,
  BUTTON_4,
  BUTTON_5,
  BUTTON_6,
  BUTTON_7,
  BUTTON_MAX = BUTTON_7,
  BUTTON_UNKNOWN
};
uint8_t button;

/*
 * Handling
 */

static uint8_t push_flag;
static uint8_t release_flag;

static uint8_t push_time[SIZE];
static uint8_t release_time[SIZE];

static uint8_t time;


static void init_timer ()
{
  /*CTC mode with OCR5A*/
  TCCR5B |= (1 << WGM52);

  /*enable output compare A interrupt*/
  TIMSK5 |= (1 << OCIE5A);

  SLOW_MODE;
}

static void init_adc ()
{
  /*
   * Enable adc, enable interrupt,
   * prescaler: 16mHz/128 = 125kHz
   */
  ADCSRA |= (1 << ADEN) | (1 << ADIE)
    | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

  /*ref voltage: AVCC, right adjust result, 8 bits are used*/
  ADMUX |= (1 << REFS0) | (1 << ADLAR);

  /*disable digital input on adc pins 0-7*/
  DIDR0 = 0xFF;
}

static void reset ()
{
  /*call after msg sent*/
  /*fixme: */
  push_flag = release_flag = 0;
  uint8_t i = 0;
  for (i = 0; i < SIZE; ++i)
    push_time[i] = release_time[i] = 0;
}

static void start_button ()
{
  /*stop for a moment*/
  STOP_TIMER;
  
  /*adjust adc for write input*/
  SELECT_BUTTON (button);

  /*change voltage level at right pin*/
  SRC_HIGH (button);

  /*wait a bit, to measure right voltage*/
  FAST_MODE;
}

static void finish_button ()
{
  SRC_LOW (button);
  uint8_t voltage = ADCH;
  uint8_t mask = (1 << button);
  if ((voltage < VOLTAGE_LOW_THRESHOLD)
      && ((push_flag & mask) == 0)) {
    push_flag |= mask;
    push_time[button] = time;
  } else if ((voltage > VOLTAGE_HIGH_THRESHOLD)
             && ((push_flag & mask) != 0)
             && ((release_flag & mask) == 0)) {
    release_flag |= mask;
    release_time[button] = time;
  }
}

static void start_adc ()
{
  STOP_TIMER;
  ADCSRA |= (1 << ADSC);
}

enum {
  SHORT_PRESS,
  LONG_PRESS
};
uint8_t process_time (uint8_t push, uint8_t release)
{
  uint8_t delta = (push <= release) ? release - push
    : (0xFF - push + release);

  return (delta <= TIME_THRESHOLD) ? SHORT_PRESS : LONG_PRESS;
}

static void process_data ()
{
  if ((push_flag == 0)
      || (push_flag != release_flag))
    return;

  uint8_t short_press = 0;
  uint8_t long_press = 0;
  uint8_t i;
  for (i = 0; i < SIZE; ++i) {
    uint8_t mask = (1 << i);
    if ((push_flag & mask) != 0) {
      uint8_t bias = process_time (push_time[i], release_time[i]);
      switch (bias) {
      case SHORT_PRESS:
        short_press |= mask;
        break;
      case LONG_PRESS:
        long_press |= mask;
        break;
      default:
        break;
      }
    }
  }

  /*either short or long should not be zero*/
  encode_msg_2 (MSG_ID_BUTTON, 0, short_press, long_press);

  reset ();
}

/*----------------------------------------*/

void button_init ()
{
  /*fixme*/
  time = 0;
  button = BUTTON_UNKNOWN;

  init_timer ();
  init_adc ();

  /*configure whole port A as output*/
  PORTA = 0xFF;
}

ISR (TIMER5_COMPA_vect)
{
  if (button == BUTTON_UNKNOWN) {
    button = BUTTON_0;
    start_button ();
    return;
  }

  /*
   * We can get here only if we are waiting for the middle of the front
   */
  start_adc ();
}

ISR (ADC_vect)
{
  finish_button ();
  if (button < BUTTON_MAX) {
    ++button;
    start_button ();
    return;
  }

  process_data ();
  ++time;
  button = BUTTON_UNKNOWN;
  SLOW_MODE;
}
