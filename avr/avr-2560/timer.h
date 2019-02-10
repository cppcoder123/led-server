/*
 * Share timer functionality between display messages
 * arriving from r-pi and internal clock running
 * when r-pi is powered off
 */
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_disable ();


#define TIMER_TEN_PER_SECOND 1562
#define TIMER_ONE_PER_SECOND 15625

typedef void (*timer_function) ();

void timer_enable (uint16_t delay, timer_function fun);

#endif
