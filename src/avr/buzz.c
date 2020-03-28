/*
 *
 */

#include <avr/io.h>

/* #include "unix/constant.h" */

/* #include "buf.h" */
#include "buzz.h"
#include "counter.h"
/* #include "cron.h" */
/* #include "debug.h" */

#define BUZZ_COUNTER COUNTER_0
/* #define BUZZ_PRESCALER COUNTER_PRESCALER_1024 */
#define BUZZ_PRESCALER COUNTER_PRESCALER_8
#define BUZZ_PWM 1

#define BUZZ_ZERO 0

/* #define NOTE_SIZE 3 */

#define BUZZ_PORT PORTG5

/* static uint8_t in_progress = 0; */
/* static uint8_t repeat_counter = 0; */
/* static uint8_t repeat_limit = 0; */

/* static struct buf_t melody; */
/* static uint8_t melody_position = 0; */

/* static void handle_sound (); */

void buzz_init ()
{
  /* in_progress = 0; */
  /* repeat_counter = 0; */
  /* repeat_limit = 0; */
  /* buf_init (&melody); */
  /* melody_position = 0; */

  /* enable buzz output */
  DDRG |= (1 << BUZZ_PORT);
}

/* /\* it returns 0 in case of failure *\/ */
/* static uint8_t get_sound (uint8_t *duration, uint8_t *pitch, uint8_t *volume) */
/* { */
/*   if (melody_position + NOTE_SIZE > buf_size (&melody)) { */
/*     if (++repeat_counter >= repeat_limit) */
/*       return 0; */
/*     melody_position = 0; */
/*   } */

/*   debug_0 (DEBUG_BUZZ, 22); */

/*   if ((buf_byte_get (&melody, melody_position++, pitch) == 0) */
/*       || (buf_byte_get (&melody, melody_position++, volume) == 0) */
/*       || (buf_byte_get (&melody, melody_position++, duration) == 0)) */
/*     return 0; */

/*   return 1; */
/* } */

/* static void set_pitch_volume (uint8_t pitch, uint8_t volume) */
/* { */
/*   /\* fixme *\/ */
/*   counter_register_write (BUZZ_COUNTER, */
/*                           COUNTER_OUTPUT_COMPARE_A, pitch, BUZZ_ZERO); */
/*   counter_register_write */
/*     (BUZZ_COUNTER, COUNTER_OUTPUT_COMPARE_B, */
/*      (volume < pitch) ? volume : pitch, BUZZ_ZERO); */
/*   counter_pwm (1, BUZZ_COUNTER, BUZZ_PWM); */
/*   counter_enable (BUZZ_COUNTER, BUZZ_PRESCALER); */
/* } */

/* static void duration_callback () */
/* { */
/*   counter_pwm (0, BUZZ_COUNTER, BUZZ_PWM); */
/*   counter_disable (BUZZ_COUNTER); */
/*   cron_disable (CRON_ID_BUZZ); */

/*   handle_sound (); */
/* } */

/* static void handle_sound () */
/* { */
/*   uint8_t duration = 0, pitch = 0, volume = 0; */

/*   /\* debug_0 (DEBUG_BUZZ, 33); *\/ */

/*   if (get_sound (&duration, &pitch, &volume) == 0) */
/*     return; */

/*   debug_3 (DEBUG_BUZZ, 11, duration, pitch, volume); */

/*   if ((pitch != 0) && (volume != 0)) */
/*     set_pitch_volume (pitch, volume); */

/*   cron_enable (CRON_ID_BUZZ, duration, duration_callback); */
/* } */

void buzz_start ()
{
  counter_register_write (BUZZ_COUNTER,
                          COUNTER_OUTPUT_COMPARE_A, 250, BUZZ_ZERO);
  counter_register_write
    (BUZZ_COUNTER, COUNTER_OUTPUT_COMPARE_B, 125, BUZZ_ZERO);
  counter_pwm (1, BUZZ_COUNTER, BUZZ_PWM);
  counter_enable (BUZZ_COUNTER, BUZZ_PRESCALER);
}

void buzz_stop ()
{
  counter_pwm (0, BUZZ_COUNTER, BUZZ_PWM);
  counter_disable (BUZZ_COUNTER);
}

/* #if 0 */
/* void buzz_start () */
/* { */
/*   if (in_progress != 0) */
/*     return; */

/*   melody_position = 0; */

/*   repeat_counter = 0; */

/*   handle_sound (); */
/* } */

/* void buzz_stop () */
/* { */
/*   if (in_progress == 0) */
/*     return; */

/*   counter_disable (BUZZ_COUNTER); */
/*   /\* counter_disable (DURATION_COUNTER); *\/ */
/* } */
/* #endif */

/* void buzz_clear () */
/* { */
/*   if (in_progress != 0) */
/*     return; */

/*   buf_clear (&melody); */
/*   repeat_limit = 0; */

/*   melody_position = 0; */
/* } */

/* uint8_t buzz_add_sound (uint8_t pitch, uint8_t volume, uint8_t duration) */
/* { */
/*   if (buf_space (&melody) < NOTE_SIZE) */
/*     return 0; */

/*   if (volume > pitch) */
/*     volume = pitch; */

/*   return ((buf_byte_fill (&melody, pitch) == 0) */
/*           || (buf_byte_fill (&melody, volume) == 0) */
/*           || (buf_byte_fill (&melody, duration) == 0)) */
/*     ? 0 : 1; */
/* } */

/* uint8_t buzz_add_pause (uint8_t duration) */
/* { */
/*   return buzz_add_sound (0, 0, duration); */
/* } */

/* void buzz_repeat (uint8_t num) */
/* { */
/*   repeat_limit = num; */
/* } */
