/*
 *
 */

#include "feedback.h"

void feedback_init (struct feedback_t *feedback, uint8_t target, uint8_t delta,
                    uint8_t delay, uint8_t ignore, feedback_control control)
{
  feedback->target = target;
  feedback->delta = delta;
  feedback->delay = ignore;
  feedback->max_delay = delay;
  buf_init (&feedback->data);
  feedback->control = control;
}

void feedback_try (struct feedback_t *feedback)
{
  uint8_t current;
  if (buf_byte_drain (&feedback->data, &current) == 0)
    return;
  if (feedback->delay > 1) {
    --feedback->delay;
    return;
  }

  feedback->delay = feedback->max_delay;

  uint8_t delta = (current > feedback->target)
    ? (current - feedback->target) : (feedback->target - current);

  if (delta > feedback->delta)
    feedback->control (current);
}

void feedback_data (struct feedback_t *feedback, uint8_t data)
{
  buf_byte_fill (&feedback->data, data);
}
