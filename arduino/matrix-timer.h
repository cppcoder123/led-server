/*
 *
 *
 */
#ifndef MATRIX_TIMER_H
#define MATRIX_TIMER_H

#include <stdint.h>

void matrix_timer_init ();

volatile uint8_t* matrix_timer_get_condition ();

#endif
