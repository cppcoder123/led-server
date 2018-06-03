/*
 *
 *
 *
 */

#include "matrix-timer.h"

volatile uint8_t matrix_timer_condition;

void matrix_timer_init ()
{
  /* no op for now, fixme */
}

volatile uint8_t* matrix_timer_get_condition ()
{
  return &matrix_timer_condition;
}

