/*
 *
 */
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/* all our timers */
enum {
    TIMER_0,
    TIMER_1,
    TIMER_2,
    TIMER_3,
    TIMER_4,
    TIMER_5,
};

/* prescaler values */
#define TIMER_PRESCALER_0 (0)   /* default value, disables the timer */
#define TIMER_PRESCALER_1 (1 << 0)
#define TIMER_PRESCALER_8 (1 << 1)
#define TIMER_PRESCALER_64 (TIMER_PRESCALER_1 | TIMER_PRESCALER_8)
#define TIMER_PRESCALER_256 (1 << 2)
#define TIMER_PRESCALER_1024 (TIMER_PRESCALER_256 | TIMER_PRESCALER_1)
/* ext clock signal on Tn pin*/
#define TIMER_PRESCALER_EXT_FALL (TIMER_PRESCALER_256 | TIMER_PRESCALER_8)
#define TIMER_PRESCALER_EXT_RISE (TIMER_PRESCALER_256 |
TIMER_PRESCALER_8 | TIMER_PRESCALER_1)

/* interrupt type values */
#define TIMER_INTERRUPT_OVERFLOW (1 << 0)
#define TIMER_INTERRUPT_COMPARE_A (1 << 1)

/* interrupt action */
typedef void (*interrupt_function) ();

/* init internal structure */
void timer_init ();

/* timer can't be enabled w/o prescaler set first */
void timer_enable (uint8_t id);
void timer_disable (uint8_t id);

void timer_prescaler (uint8_t id, uint8_t prescaler);

void timer_interrupt (uint8_t id, uint8_t int_type, interrupt_function fun);

/* only low is valid for timer 0, 2*/
void timer_set (uint8_t id, uint8_t low, uint8_t high);
void timer_get (uint8_t id, uint8_t *low, uint8_t *high);

#endif
