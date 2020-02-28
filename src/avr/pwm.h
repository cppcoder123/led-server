/*
 *
 */
#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#include "buf.h"

typedef void (*pwm_control) (uint8_t current);

struct pwm_t
{
  uint8_t target;
  uint8_t delta;
  volatile struct buf_t feedback;
  uint8_t delay;
  uint8_t max_delay;
  pwm_control control;
};

void pwm_init (struct pwm_t *pwm, uint8_t target,
               uint8_t delta, uint8_t delay, pwm_control control);

/* check current value and call 'control' if needed */
void pwm_try (struct pwm_t *pwm);

/* provide current value for analysis */
void pwm_current (struct pwm_t *pwm, uint8_t current);

#endif
