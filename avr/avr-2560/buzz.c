/*
 *
 */

#include <avr/interrupt.h>
#include <util/atomic.h>

#include "buzz.h"
#include "ring.h"

/*1 => 16Mhz*/
#define PITCH_PRESCALER (1 << CS30)
/*1024, max*/
#define DURATION_PRESCALER ((1 << CS40) | (1 << CS42))

#define PITCH_TIMER_ENABLE TCCR3B |= PITCH_PRESCALER
#define PITCH_TIMER_DISABLE TCCR3B &= ~PITCH_PRESCALER

#define DURATION_TIMER_ENABLE TCCR4B |= DURATION_PRESCALER
#define DURATION_TIMER_DISABLE TCCR4B &= ~DURATION_PRESCALER

/*highest freq 5khz => 3200*/
/* #define MIN_PITCH 1600 */
#define MIN_FREQUENCY 1600
/*lowest freq 2khz => 8000*/
/* #define MAX_PITCH 4000 */
/*so pitch step is 9*/
#define FREQUENCY_STEP 9

#define PITCH_REGISTER OCR3A
#define DURATION_REGISTER OCR4A

/*16mhz / 1024 ~ 15625khz, we want 0.1 step so*/
#define DURATION_STEP 1562

#define BUZZ_PIN PH7
#define PIN_TOGGLE PORTH ^= (1 << BUZZ_PIN)
#define PIN_LOW PORTH &= ~(1 << BUZZ_PIN)

#define SOUND_SIZE 128

volatile data_t sound[SOUND_SIZE];
volatile uint8_t mode;
static data_t pause;

enum {
  IDLE,
  WORKING
};
volatile uint8_t mode;

void buzz_init ()
{
  /* fixme */

  ring_init (sound, SOUND_SIZE);
  mode = IDLE;
  /*
   *
   */

  /*CTC mode with OCR13A (and OCR14A)*/
  TCCR3B |= (1 << WGM32);
  TCCR4B |= (1 << WGM42);

  /*enable interrupts*/
  TIMSK3 |= (1 << OCIE3A);
  TIMSK4 |= (1 << OCIE4A);
}

uint8_t buzz_add_data (data_t pitch,
                       data_t sound_duration,
                       data_t silence_duration)
{
  return ((ring_symbol_fill (sound, pitch) != 0)
          && (ring_symbol_fill (sound, sound_duration) != 0)
          && (ring_symbol_fill (sound, silence_duration) != 0))
    ? 1 : 0;
}

void buzz_try ()
{
  if ((mode != IDLE)
      || (ring_size (sound) < 3))
    return;

  mode = WORKING;
  
  uint8_t pitch;
  uint8_t duration;
  ring_symbol_drain (sound, &pitch);
  ring_symbol_drain (sound, &duration);
  ring_symbol_drain (sound, &pause);

  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    PITCH_REGISTER = MIN_FREQUENCY + (FREQUENCY_STEP * pitch);
    DURATION_REGISTER = DURATION_STEP * duration;
  }
  PITCH_TIMER_ENABLE;
  DURATION_TIMER_ENABLE;
}

ISR (TIMER3_COMPA_vect)
{
  PIN_TOGGLE;
}

ISR (TIMER4_COMPA_vect)
{
  PITCH_TIMER_DISABLE;

  PIN_LOW;

  if (pause != 0) {
    DURATION_REGISTER = DURATION_STEP * pause;
    pause = 0;
  } else {
    DURATION_TIMER_DISABLE;
    mode = IDLE;
  }
}
