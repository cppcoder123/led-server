/*
 *
 */
#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>

/* all our counters */
enum {
    COUNTER_0,
    COUNTER_1,
    COUNTER_2,
    COUNTER_3,
    COUNTER_4,
    COUNTER_5,
};

/* prescaler values */
#define COUNTER_PRESCALER_0 (0)   /* default value, disables the counter */
#define COUNTER_PRESCALER_1 (1 << 0)
#define COUNTER_PRESCALER_8 (1 << 1)
#define COUNTER_PRESCALER_64 (COUNTER_PRESCALER_1 | COUNTER_PRESCALER_8)
#define COUNTER_PRESCALER_256 (1 << 2)
#define COUNTER_PRESCALER_1024 (COUNTER_PRESCALER_256 | COUNTER_PRESCALER_1)
/* ext clock signal on Tn pin*/
#define COUNTER_PRESCALER_EXT_FALL (COUNTER_PRESCALER_256 | COUNTER_PRESCALER_8)
#define COUNTER_PRESCALER_EXT_RISE (COUNTER_PRESCALER_256 \
                                    | COUNTER_PRESCALER_8 | COUNTER_PRESCALER_1)

/* interrupt type values */
#define COUNTER_INTERRUPT_OVERFLOW (1 << 0)
#define COUNTER_INTERRUPT_COMPARE_A (1 << 1)

/* interrupt action */
typedef void (*counter_handle) ();

/* init internal structure */
void counter_init ();

/* counter can't be enabled w/o prescaler set first */
void counter_enable (uint8_t id);
void counter_disable (uint8_t id);

void counter_prescaler (uint8_t id, uint8_t prescaler);

void counter_interrupt (uint8_t id, uint8_t int_type, counter_handle fun);

/* only low is valid for counter 0, 2*/
void counter_set (uint8_t id, uint8_t low, uint8_t high);
void counter_get (uint8_t id, uint8_t *low, uint8_t *high);

#endif
