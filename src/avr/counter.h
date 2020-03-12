/*
 *
 */
#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>

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

/* interrupt action */
typedef void (*counter_handle) ();

/* all our counters */
enum {
    COUNTER_0,
    COUNTER_1,
    COUNTER_2,
    COUNTER_3,
    COUNTER_4,
    COUNTER_5,
};

/* registers we want to update */
enum {
    COUNTER_OUTPUT_COMPARE_A,
    COUNTER_OUTPUT_COMPARE_B,
    /* output_compare_c is not used */
};

/* init internal structure */
void counter_init ();

void counter_enable (uint8_t id, uint8_t prescaler);
void counter_disable (uint8_t id);

/* only ctc compare-a interrupts are used now */
void counter_interrupt_enable (uint8_t id, counter_handle fun);
void counter_interrupt_disable (uint8_t id);

/* only low is valid for counter 0, 2*/
void counter_set_register (uint8_t id, uint8_t reg, uint8_t low, uint8_t high);

/* perform tasks scheduled by interrupts */
void counter_try ();

#endif
