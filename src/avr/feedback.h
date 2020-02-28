/*
 *
 */
#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <stdint.h>

#include "buf.h"

typedef void (*feedback_control) (uint8_t current);

struct feedback_t
{
  uint8_t target;
  uint8_t delta;
  volatile struct buf_t data;
  uint8_t delay;
  uint8_t max_delay;
  feedback_control control;
};

void feedback_init (struct feedback_t *feedback, uint8_t target,
                    uint8_t delta, uint8_t delay, feedback_control control);

/* check current value and call 'control' if needed */
void feedback_try (struct feedback_t *feedback);

/* provide data for analysis */
void feedback_data (struct feedback_t *feedback, uint8_t data);

#endif
