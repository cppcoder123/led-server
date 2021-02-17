/*
 *
 */

#include "feedback.h"

void feedback_init (struct feedback_t *feedback, uint8_t target,
                    uint8_t delta, uint8_t ignore, feedback_control control)
{
  feedback->target = target;
  feedback->delta = delta;
  feedback->ignore = ignore;
  buf_init (&feedback->data);
  feedback->control = control;
}

void feedback_try (struct feedback_t *feedback)
{
  uint8_t current;
  if (buf_byte_drain (&feedback->data, &current) == 0)
    return;
  if (feedback->ignore > 1) {
    --feedback->ignore;
    return;
  }

  uint8_t delta = (current > feedback->target)
    ? (current - feedback->target) : (feedback->target - current);

  if (delta > feedback->delta)
    feedback->control (current);
}

void feedback_data (struct feedback_t *feedback, uint8_t data)
{
  buf_byte_fill (&feedback->data, data);
}
