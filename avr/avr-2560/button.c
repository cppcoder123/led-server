/*
 *
 */
#include <avr/interrupt.h>
#include <stdint.h>

#include "mcu/constant.h"

#include "button.h"
#include "encode.h"

#define TRUE 1
#define FALSE 0

/* 0, 1 .. 7 => 8 bits */
#define WORD_SIZE 8

#define HANDLE_BEFORE_PULSE 0
#define HANDLE_PULSE 1
#define HANDLE_BEFORE_PROCESS 2
#define HANDLE_PROCESS 3

#define STATE_LENGTH 2

/*12 touch pads, 1 is for reference, so 0-10 => 11 buttons*/
#define MIN_BUTTON 0
#define MAX_BUTTON 10

#define NUM_CYCLES (MAX_BUTTON + 1)

#define MUX3_MASK ((1 << MUX0) | (1 << MUX1) | (1 << MUX2))

#define DRIVE_PIN_MASK (1 << PORTA0)

volatile uint8_t handle;

volatile uint8_t is_button_pressed;

// 9 buttons => 1 char is not enough
static uint8_t prev_state[STATE_LENGTH];
static uint8_t current_state[STATE_LENGTH];

static uint8_t button_state[STATE_LENGTH];

static uint8_t interval;

static uint8_t current_button;


static void state_copy (uint8_t *src, uint8_t *dst)
{
  *dst = *src;
  *(dst + 1) = *(src + 1);
}

static uint8_t state_equal (uint8_t *a, uint8_t *b)
{
  return ((*a == *b) && (*(a + 1) == *(b + 1))) ? TRUE : FALSE;
}

static void state_update (uint8_t *src, uint8_t *dst)
{
  /* 'or' and save to dst*/
  *dst |= *src;
  *(dst + 1) |= *(src + 1);
}

static uint8_t state_is_zero (uint8_t *s)
{
  return ((*s == 0) && (*(s+1) == 0)) ? TRUE : FALSE;
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

static void pulse (uint8_t bool_value)
{
  if (bool_value == TRUE)
    PORTA |= DRIVE_PIN_MASK;
  else
    PORTA &= ~DRIVE_PIN_MASK;
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
  /* we need to divide freq by 2: */
  OCR2A = 2;

  /* compare with OCRA and enable timer2 interrupt */
  TIMSK2 |= ((1 << OCIE2A) | (1 << TOIE2));
}

static void select_button ()
{
  /* fixme */

  ADMUX &= ~MUX3_MASK;
  ADCSRB &= ~(1 << MUX5);

  /*we are counting buttons from 0, so*/
  if (current_button >= WORD_SIZE)
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
  /*wait a bit before comparator switched on*/
  asm ("nop");
  asm ("nop");
  pulse (TRUE);
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

  DDRA |= DRIVE_PIN_MASK;
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
    pulse (FALSE);
    process_data ();
    handle = HANDLE_BEFORE_PULSE;
  }
}

ISR (TIMER2_COMPA_vect)
{
  /* timer value TCNT2 should be cleared by hardware, right? */
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
