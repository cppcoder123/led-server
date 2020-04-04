/*
 *
 */

#include <avr/io.h>

#include "at.h"
#include "buzz.h"
#include "counter.h"

#define BUZZ_COUNTER COUNTER_0
#define BUZZ_PRESCALER COUNTER_PRESCALER_8
#define BUZZ_PWM 1

/* 2 seconds */
#define BUZZ_DELAY 2            /* 2 seconds */

#define BUZZ_ZERO 0

#define BUZZ_MIN 2
#define BUZZ_MAX 255

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
  pitch = BUZZ_MIN;

  /* enable buzz output */
  DDRG |= (1 << BUZZ_PORT);

  set_output_zero ();
}

static void set_pitch (uint8_t pitch)
{
  uint8_t reg_b = pitch / 2;

  counter_disable (BUZZ_COUNTER);

  counter_register_write (BUZZ_COUNTER,
                          COUNTER_OUTPUT_COMPARE_A, pitch, BUZZ_ZERO);
  counter_register_write (BUZZ_COUNTER,
                          COUNTER_OUTPUT_COMPARE_B, reg_b, BUZZ_ZERO);
  counter_pwm (1, BUZZ_COUNTER, BUZZ_PWM);

  counter_enable (BUZZ_COUNTER, BUZZ_PRESCALER);
}

static void reschedule ()
{

  if ((flag & FLAG_BUZZING) == 0) {
    counter_disable (BUZZ_COUNTER);
    set_output_zero ();
    return;
  }

  if (flag | FLAG_ASCEND) {
    if (pitch < BUZZ_MAX)
      ++pitch;
    else
      flag &= ~FLAG_ASCEND;     /* start descend */
  } else {
    if (pitch > BUZZ_MIN)
      --pitch;
    else
      flag |= FLAG_ASCEND;
  }

  set_pitch (pitch);

  at_schedule (AT_BUZZ, BUZZ_DELAY, &reschedule);
}

void buzz_start ()
{
  flag |= (FLAG_BUZZING | FLAG_ASCEND);
  pitch = BUZZ_MIN;
  set_pitch (pitch);

  at_schedule (AT_BUZZ, BUZZ_DELAY, &reschedule);
}

void buzz_stop ()
{
  flag &= ~FLAG_BUZZING;
}
