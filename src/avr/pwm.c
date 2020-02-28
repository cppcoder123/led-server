/*
 *
 */

#include "pwm.h"

void pwm_init (struct pwm_t *pwm, uint8_t target,
               uint8_t delta, uint8_t delay, pwm_control control)
{
  pwm->target = target;
  pwm->delta = delta;
  buf_init (&pwm->feedback);
  pwm->delay = 0;
  pwm->max_delay = delay;
  pwm->control = control;
}

void pwm_try (struct pwm_t *pwm)
{
  uint8_t current;
  if ((buf_byte_drain (&pwm->feedback, &current) == 0)
      || (++pwm->delay < pwm->max_delay))
    return;

  uint8_t delta = (current > pwm->target)
    ? (current - pwm->target) : (pwm->target - current);

  if (delta > pwm->delta)
    pwm->control (delta);
}

void pwm_current (struct pwm_t *pwm, uint8_t current)
{
  buf_byte_fill (&pwm->feedback, current);
}
