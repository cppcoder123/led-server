/*
 *
 */

#include <avr/interrupt.h>
#include <util/atomic.h>

#include "buf.h"
#include "buzz.h"
#include "counter.h"

/*
 * !!!
 *
 *  fixme
 *
 * Counter 0 is used in sync
 * So, these things are required
 *   1. Organize shared counter (second-counter, "second" module?)
 *     1.a COUNTER_2 should be used, because COUNTER_0 drives buzz
 *   2. Write a function that will be called 100 (or 256?) times per second
 *      2.a this function should call sync & buzz callbacks
 */

#define SOUND_COUNTER COUNTER_0
#define DURATION_COUNTER COUNTER_2

#define SOUND_SIZE 5
#define PAUSE_SIZE 3

enum {
  EOF,
  PAUSE,
  SOUND,
};

static uint8_t in_progress = 0;
static volatile uint8_t repeat_counter = 0;
static volatile uint8_t repeat_limit = 0;

static volatile struct buf_t melody;
static volatile uint8_t melody_position = 0;

void buzz_init()
{
  /* fixme */
}

/* it returns either SOUND, PAUSE or EOF */
static uint8_t get_sound(uint16_t *duration, uint8_t *pitch, uint8_t *volume)
{
  if (melody_position >= buf_size(&melody)) {
    if (++repeat_counter >= repeat_limit)
      return EOF;
    melody_position = 0;
  }

  uint8_t what = 0;
  if ((buf_byte_get(&melody, melody_position++, &what) == 0)
      || ((what != SOUND)
          && (what != PAUSE))) /* either sound or silence */
    return EOF;              /* internal error */

  if (what == SOUND) {
    if ((buf_byte_get(&melody, melody_position++, pitch) == 0)
        || (buf_byte_get(&melody, melody_position++, volume) == 0))
      return EOF;         /* internal error */
  }
  uint8_t high = 0, low = 0;
  if ((buf_byte_get(&melody, melody_position++, &high) == 0)
      || (buf_byte_get(&melody, melody_position++, &low) == 0))
    return EOF;             /* internal error */

  *duration = (((uint16_t) high) << 8) + ((uint16_t) low);

  return what;
}

static void set_pitch_volume(uint8_t pitch, uint8_t volume)
{
  /* fixme */
}

static void set_duration(uint16_t duration)
{
  /* fixme */
}

void buzz_start()
{
  if (in_progress != 0)
    return;

  repeat_counter = 0;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    uint16_t duration = 0;
    uint8_t pitch = 0, volume = 0;
    switch(get_sound(&duration, &pitch, &volume)) {
    case EOF:
      return;
    case SOUND:
      set_pitch_volume(pitch, volume);
      counter_enable(SOUND_COUNTER);
      /* intentionally no break here */
    case PAUSE:
      set_duration(duration);
      counter_enable(DURATION_COUNTER);
      break;
    default:
      break;
    }
  }
}

void buzz_stop()
{
  if (in_progress == 0)
    return;

  counter_disable(SOUND_COUNTER);
  counter_disable(DURATION_COUNTER);
}

void buzz_clear()
{
  if (in_progress != 0)
    return;

  buf_clear(&melody);
  repeat_limit = 0;

  melody_position = 0;
}

static void split_16_bit(uint16_t src, uint8_t *high, uint8_t *low)
{
  *low = (uint8_t) (src & 0xFF);
  *high = (uint8_t) (src >> 8);
}

uint8_t buzz_add_sound(uint8_t pitch, uint8_t volume, uint16_t duration)
{
  if (buf_space(&melody) < SOUND_SIZE)
    return 0;

  uint8_t status = 0;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (volume > pitch)
      volume = pitch;

    uint8_t high = 0, low = 0;
    split_16_bit(duration, &high, &low);

    status = ((buf_byte_fill(&melody, SOUND) == 0)
              || (buf_byte_fill(&melody, pitch) == 0)
              || (buf_byte_fill(&melody, volume) == 0)
              || (buf_byte_fill(&melody, high) == 0)
              || (buf_byte_fill(&melody, low) == 0))
      ? 0 : 1;
  }

  return status;
}

uint8_t buzz_add_pause(uint16_t duration)
{
  if (buf_space(&melody) < PAUSE_SIZE)
    return 0;

  uint8_t status = 0;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    uint8_t high = 0, low = 0;
    split_16_bit(duration, &high, &low);
    status = ((buf_byte_fill(&melody, PAUSE) == 0)
              || (buf_byte_fill(&melody, high) == 0)
              || (buf_byte_fill(&melody, low) == 0))
      ? 0 : 1;
  }

  return status;
}

void buzz_repeat(uint8_t num)
{
  repeat_limit = num;
}
    

#if 0
/*
 *
 * Remove this code later
 *
 */
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

#define BUZZ_PIN PH0
#define PIN_TOGGLE PORTH ^= (1 << BUZZ_PIN)
#define PIN_LOW PORTH &= ~(1 << BUZZ_PIN)

#define SOUND_SIZE 128

volatile uint8_t sound[SOUND_SIZE];
volatile uint8_t mode;
static uint8_t pause;

enum {
  IDLE,
  WORKING
};
volatile uint8_t mode;

void buzz_init ()
{
  /* fixme: disabled */
  return;

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

  /* debug */
  ring_symbol_fill (sound, 255);
  ring_symbol_fill (sound, 5);
  ring_symbol_fill (sound, 10);
}

uint8_t buzz_add_data (uint8_t pitch,
                       uint8_t sound_duration,
                       uint8_t silence_duration)
{
  /*fixme: disabled*/
  return 1;
  
  return ((ring_symbol_fill (sound, pitch) != 0)
          && (ring_symbol_fill (sound, sound_duration) != 0)
          && (ring_symbol_fill (sound, silence_duration) != 0))
    ? 1 : 0;
}

void buzz_try ()
{
  /*fixme: disabled now*/
  return;
  
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
#endif
