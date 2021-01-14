/*
 *
 */

#include <avr/io.h>

#include "unix/constant.h"

/* #include "at.h" */
#include "buzz.h"
#include "counter.h"
#include "cron.h"
#include "debug.h"

#define BUZZ_COUNTER COUNTER_4
#define BUZZ_PRESCALER COUNTER_PRESCALER_8
#define BUZZ_PWM 1

/* 2 seconds */
/* #define BUZZ_DELAY 2            /\* 2 seconds *\/ */
#define BUZZ_DELAY 100            /* 2 seconds */

#define BUZZ_ZERO 0

#define PITCH_STEP 5
#define PITCH_MIN 125
#define PITCH_MAX 250

#define BUZZ_PORT PORTG5

#define FLAG_BUZZING (1 << 0)   /* else no buzz */
#define FLAG_ASCEND (1 << 1)    /* else descend */

static uint8_t flag = 0;

static uint8_t pitch = 0;

static void set_output_zero ()
{
  PORTG &= (1 << BUZZ_PORT);
}

void buzz_init ()
{
  flag = 0;
  pitch = PITCH_MAX;

  /* enable buzz output */
  DDRG |= (1 << BUZZ_PORT);

  set_output_zero ();
}

static void set_pitch (uint8_t pitch)
{
  uint8_t reg_b = pitch / 2;

  counter_register_write (BUZZ_COUNTER,
                          COUNTER_OUTPUT_COMPARE_A, pitch, BUZZ_ZERO);
  counter_register_write (BUZZ_COUNTER,
                          COUNTER_OUTPUT_COMPARE_B, reg_b, BUZZ_ZERO);
  counter_pwm (1, BUZZ_COUNTER, BUZZ_PWM);

  counter_enable (BUZZ_COUNTER, BUZZ_PRESCALER);
}

static void reschedule ();

static void reschedule ()
{
  /* debug_0 (DEBUG_BUZZ, 111); */

  counter_disable (BUZZ_COUNTER);

  if ((flag & FLAG_BUZZING) == 0) {
    set_output_zero ();
    return;
  }

  if ((flag & FLAG_ASCEND) != 0) {
    if (pitch < PITCH_MAX)
      pitch += PITCH_STEP;
    else
      flag &= ~FLAG_ASCEND;     /* start descend */
  } else {
    if (pitch > PITCH_MIN)
      pitch -= PITCH_STEP;
    else
      flag |= FLAG_ASCEND;
  }

  /* debug_1 (DEBUG_BUZZ, 22, pitch); */
  set_pitch (pitch);
  /* debug_0 (DEBUG_BUZZ, 23); */

  /* at_schedule (AT_BUZZ, BUZZ_DELAY, &reschedule); */
  /* debug_0 (DEBUG_BUZZ, 24); */
}

void buzz_start ()
{
  flag |= (FLAG_BUZZING);
  flag &= ~(FLAG_ASCEND);
  pitch = PITCH_MAX;
  set_pitch (pitch);

  cron_enable (CRON_ID_BUZZ, BUZZ_DELAY, &reschedule);
}

void buzz_stop ()
{
  flag &= ~FLAG_BUZZING;
  cron_disable (CRON_ID_BUZZ);
  /* counter_disable (BUZZ_COUNTER); */
}
