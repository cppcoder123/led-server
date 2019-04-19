/*
 *
 */
#include <avr/interrupt.h>
#include <stdint.h>

#include "mcu/constant.h"

#include "button.h"
#include "encode.h"
#include "data-type.h"

#define TRUE 1
#define FALSE 0

#define HANDLE_BEFORE_PULSE 0
#define HANDLE_PULSE 1
#define HANDLE_BEFORE_PROCESS 2
#define HANDLE_PROCESS 3

/* #define HANDLE_FIRST HANDLE_BEFORE_PROCESS */
/* #define HANDLE_LAST HANDLE_PROCESS */

#define STATE_LENGTH 2

#define MIN_BUTTON 0
#define MAX_BUTTON 8

#define NUM_CYCLES (MAX_BUTTON + 1)

#define MUX3_MASK ((1 << MUX0) | (1 << MUX1) | (1 << MUX2))

volatile data_t handle;

volatile data_t is_button_pressed;

// 9 buttons => 1 char is not enough
static data_t prev_state[STATE_LENGTH];
static data_t current_state[STATE_LENGTH];

static data_t button_state[STATE_LENGTH];

static data_t interval;

static data_t current_button;


static void state_copy (data_t *src, data_t *dst)
{
  *dst = *src;
  *(dst + 1) = *(src + 1);
}

static data_t state_equal (data_t *a, data_t *b)
{
  return ((*a == *b) && (*(a + 1) == *(b + 1))) ? TRUE : FALSE;
}

static void state_update (data_t *src, data_t *dst)
{
  /* 'or' and save to dst*/
  *dst |= *src;
  *(dst + 1) |= *(src + 1);
}

static data_t state_is_zero (data_t *s)
{
  return ((*s == 0) && (*(s+1) == 0)) ? TRUE : FALSE;
}

static void state_init (data_t *s)
{
  *s = 0;
  *(s + 1) = 0;
}

static void state_raise_bit (data_t *s, data_t bit)
{
  if (bit <= 7)
    *s |= (1 << bit);
  else
    *(s + 1) |= (1 << (bit - 7));
}

static void send_message ()
{
  encode_msg_3 (MSG_ID_BUTTON, SERIAL_ID_TO_IGNORE,
                button_state[0], button_state[1], interval);
}

static void process_data_button ()
{
  if (is_button_pressed == TRUE)
    state_raise_bit (current_state, current_button);

  ++current_button;
}

static void process_data_array ()
{
  if (state_equal (current_state, prev_state) == FALSE)
    // smth is pressed or released
    state_update (current_state, button_state);

  if (state_is_zero (current_state) == FALSE)
    // at least one button is pressed
      ++interval;
  else if (state_is_zero (button_state) == FALSE) {
    /* all button released now but there was a press before, */
    /* so we need to generate a message */
    send_message ();
    interval = 0;
    state_init (button_state);
    state_init (current_state);
  }

  state_copy (current_state, prev_state);
  state_init (current_state);
  current_button = MIN_BUTTON;
}

static void enable_comparator ()
{
  /* fixme */
  /* enable comparator*/
  ADCSRB |= (1 << ACME);

  /* enable interrupt, interrupt on rising */
  ACSR |= (1 << ACIE) | (1 << ACIS1) | (1 << ACIS0);

  /*disable digital input*/
  DIDR1 |= (1 << AIN1D) | (1 << AIN0D);
}

static void disable_comparator ()
{
  /* fixme ?*/
  ADCSRB &= ~(1 << ACME);
}

static void timer_init ()
{
  /* 1024 clock prescaler*/
  TCCR2B |= ((1 << CS22) | (1 << CS21) | (1 << CS20));

  /* CTC with OCRA */
  TCCR2A |= (1 << WGM21);

  /* set OCRA*/
  /*we want aprox 1.5kHz, so 4mHz / 1024 = 4kHz,*/
  /* we need to divide freq by 2: 0xFF - 2 */
  OCR2A = 253;

  /* compare with OCRA and enable timer2 interrupt */
  TIMSK2 |= ((1 << OCIE2A) | (1 << TOIE2));
}

static void select_button ()
{
  /* fixme */

  ADMUX &= ~MUX3_MASK;
  ADCSRB &= ~(1 << MUX5);

  /*we are counting buttons from 0, so*/
  if (current_button > 7)
    ADCSRB |= (1 << MUX5);

  ADMUX |= (current_button & MUX3_MASK);
}

static void process_data ()
{
  if (current_button == NUM_CYCLES)
    process_data_array ();
  else
    process_data_button ();
}

static void generate_pulse ()
{
  if (current_button == NUM_CYCLES) {
    disable_comparator ();
    return;
  }

  disable_comparator ();
  select_button ();
  is_button_pressed = FALSE;
  enable_comparator ();
}

void button_init ()
{
  handle = HANDLE_BEFORE_PULSE;
  interval = 0;
  state_init (current_state);
  state_init (prev_state);
  state_init (button_state);
  current_button = MIN_BUTTON;

  timer_init ();
}

void button_try ()
{
  if ((handle != HANDLE_PULSE)
      || (handle != HANDLE_PROCESS))
    return;

  if (handle == HANDLE_PULSE) {
    generate_pulse ();
    handle = HANDLE_BEFORE_PROCESS;
  } else {
    process_data ();
    handle = HANDLE_BEFORE_PULSE;
  }
}

ISR (TIMER2_COMPA_vect)
{
  /* fixme */
  if (handle == HANDLE_BEFORE_PULSE)
    handle = HANDLE_PULSE;
  else if (handle == HANDLE_BEFORE_PROCESS)
    handle = HANDLE_PROCESS;
}

ISR (ANALOG_COMP_vect)
{
  is_button_pressed = TRUE;
}
