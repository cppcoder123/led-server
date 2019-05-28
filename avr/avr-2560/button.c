/*
 *
 */
#include <avr/interrupt.h>
#include <stdint.h>

#include "mcu/constant.h"

#include "button.h"
#include "debug.h"
#include "encode.h"

/* 0, 1 .. 7 => 8 bits */
#define WORD_SIZE 8

#define STATE_LENGTH 2

/*12 touch pads, 1 is for reference, so 0-10 => 11 buttons*/
#define MIN_BUTTON 0
#define MAX_BUTTON 12

#define NUM_CYCLES (MAX_BUTTON + 1)

#define MUX_LOW_MASK ((1 << MUX0) | (1 << MUX1) | (1 << MUX2))
#define MUX_HIGH_MASK (1 << MUX5)

#define TEST_PIN_MASK (1 << PORTA0)

/*measured_time is greater => button is pressed*/
#define TIME_THRESHOLD 200

/*
 * 'fsm' shuld change between charge and discharge to handle all buttons
 * and the it should go to report to report if smth is pressed
 */

enum fsm_t {
  FSM_BEFORE_CHARGE,
  FSM_CHARGE,
  FSM_BEFORE_DISCHARGE,
  FSM_DISCHARGE,
  FSM_REPORT            /* send message if button(s) is(are) pressed*/
};

volatile uint8_t fsm;

// button state pressed or released
// 12 buttons => 1 char is not enough
static uint8_t prev_state[STATE_LENGTH];
static uint8_t curr_state[STATE_LENGTH];

volatile uint8_t current_button;

volatile uint16_t measured_time;

static uint8_t debug;

static void state_update (uint8_t *src, uint8_t *dst)
{
  /* 'or' and save to dst*/
  *dst |= *src;
  *(dst + 1) |= *(src + 1);
}

static uint8_t state_is_zero (uint8_t *s)
{
  return ((*s == 0) && (*(s+1) == 0)) ? 1 : 0;
}

static void state_init (uint8_t *s)
{
  *s = 0;
  *(s + 1) = 0;
}

static void state_raise_bit (uint8_t *s, uint8_t bit)
{
  if (bit < WORD_SIZE)
    *s |= (1 << bit);
  else
    *(s + 1) |= (1 << (bit - WORD_SIZE));
}

static void adjust_opamp_input ()
{
  ADMUX &= ~MUX_LOW_MASK;
  ADCSRB &= ~MUX_HIGH_MASK;

  if (current_button > WORD_SIZE)
    ADCSRB |= MUX_HIGH_MASK;

  ADMUX |= (current_button & MUX_LOW_MASK);
}

static void first_button ()
{
  current_button = MIN_BUTTON;
  adjust_opamp_input ();
}

static void next_button ()
{
  ++current_button;
  if (current_button < MAX_BUTTON)
    adjust_opamp_input ();
}

static void test_pin_rise ()
{
  PORTA |= TEST_PIN_MASK;
}

static void test_pin_fall ()
{
  PORTA &= ~TEST_PIN_MASK;
}

static void enable_opamp ()
{
  ACSR |= (1 << ACIC);
}

static void disable_opamp ()
{
  ACSR &= ~(1 << ACIC);
}

static void enable_capture ()
{
  TIMSK1 |= (1 << ICIE1);
}

static void disable_capture ()
{
  TIMSK1 &= ~(1 << ICIE1);
}

static void enable_compare_a ()
{
  TCCR1B |= (1 << WGM12);

  /*TOIE1, should we set it?*/
  TIMSK1 |= (1 << OCIE1A);
}

static void disable_compare_a ()
{
  TCCR1B &= ~(1 << WGM12);
  TIMSK1 &= ~(1 << OCIE1A);
}

static void front_wait ()
{
  enable_opamp ();
  enable_capture ();

  TCNT1 = 0;
}

static void relax_wait ()
{
  enable_compare_a ();

  TCNT1 = 0;
}

static void disable_interrupt ()
{
  disable_opamp ();
  disable_capture ();
  disable_compare_a ();
}

static void process_key ()
{
  if (measured_time >= TIME_THRESHOLD)
    state_raise_bit (curr_state, current_button);

  measured_time = 0;
}

static void process_keyboard ()
{
  uint8_t prev = state_is_zero (prev_state);
  uint8_t curr = state_is_zero (curr_state);

  if ((prev != 0) && (curr != 0))
    return;

  if (curr == 0) {
    state_update (curr_state, prev_state);
    return;
  }

  /*
   * 'curr_state' is zero, all buttons are released,
   * but 'prev_state' is not zero here, so we need 
   * to send a message
   */
  encode_msg_2 (MSG_ID_BUTTON, SERIAL_ID_TO_IGNORE, prev_state[0], prev_state[1]);

  /*
   * Clear
   */
  state_init (prev_state);
  state_init (curr_state);
}

static void init_opamp ()
{
  /* output */
  DDRA |= TEST_PIN_MASK;
  
  /*disable digital input*/
  DIDR1 |= (1 << AIN1D) | (1 << AIN0D);

  /*multiplexer enable*/
  ADCSRB |= (1 << ACME);

  /*
   * select bandgap at positive input
   * ----- clear interrupt flag, ??? ----
   * ----- enable comparator interrupt (removed?), ----
   * ----- enable input capture, (enabled seprately)
   * event on rising front (2 last flags)
   */

  ACSR |= ((1 << ACBG) | (1 << ACIS1) | (1 << ACIS0));
}

static void init_timer ()
{
  /*
   * Load resistor should be 100K.
   * if button is released then relax time is ~ 2x10(-3) sec,
   * if it is pressed  then relax time is ~4x10(-3) sec,
   * so it looks 1024 is too big prescaler,
   * lets use 64 prescaler.
   * So released button should generate intr at with counter 128,
   * pressed with counter 256 or more.
   * We have 16 bit counter, so everything should be OK.
   */
  TCCR1B |= ((1 << CS11) | (1 << CS10));

  /*
   * compare a mode used to relax, so it should be more than a pressed time,
   * so, lets say 1000
   */
  OCR1A = 1000;
}

void button_init ()
{
  state_init (curr_state);
  state_init (prev_state);
  /**/
  fsm = FSM_BEFORE_CHARGE;
  first_button ();
  test_pin_fall ();
  relax_wait ();

  init_timer ();

  init_opamp ();

  debug = 0;
}

void button_try ()
{
  if (!((fsm == FSM_BEFORE_CHARGE)
        || (fsm == FSM_BEFORE_DISCHARGE)
        || (fsm == FSM_REPORT)))
    return;

  switch (fsm) {
  case FSM_BEFORE_CHARGE:
    front_wait ();
    test_pin_rise ();
    fsm = FSM_CHARGE;
    break;
  case FSM_BEFORE_DISCHARGE:
    test_pin_fall ();
    process_key ();
    next_button ();
    relax_wait ();
    fsm = FSM_DISCHARGE;
    break;
  case FSM_REPORT:
    process_keyboard ();        /* if needed */
    first_button ();
    fsm = FSM_BEFORE_CHARGE;
    break;
  default:
    break;
  }
}

ISR (TIMER1_CAPT_vect)
{
  disable_interrupt ();
  measured_time = ICR1;
  fsm = FSM_BEFORE_DISCHARGE;
}

ISR (TIMER1_COMPA_vect)
{
  disable_interrupt ();

  if (fsm == FSM_DISCHARGE)
    fsm = (current_button < MAX_BUTTON) ? FSM_BEFORE_CHARGE : FSM_REPORT;
}

/* ISR (ANALOG_COMP_vect) */
/* { */
/*   if (is_button_pressed == FALSE) */
/*     encode_msg_1 (MSG_ID_DEBUG_A, SERIAL_ID_TO_IGNORE, current_button); */
/*   is_button_pressed = TRUE; */
/* } */
