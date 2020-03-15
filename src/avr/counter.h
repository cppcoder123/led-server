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
typedef void (*counter_callback) ();

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
    COUNTER_VALUE,
};

/* init internal structure */
void counter_init ();

void counter_enable (uint8_t counter_id, uint8_t prescaler);
void counter_disable (uint8_t counter_id);

/* only ctc compare-a interrupts are used now, */
/* so output-compare-a register should be updated before */
/* calling counter_enable */
void counter_interrupt (uint8_t enable,
                        uint8_t counter_id, counter_callback fun);

/* fast pwm with output-compare-a & output-compare-b */
/* these registers should be set and !!! b <= a !!! */
/* before counter_enable call */
void counter_pwm (uint8_t enable, uint8_t counter_id);

/* only low is valid for counter 0, 2*/
void counter_register_write (uint8_t counter_id, uint8_t reg_id,
			     uint8_t low, uint8_t high);
void counter_register_read (uint8_t counter_id, uint8_t reg_id,
			    uint8_t *low, uint8_t *high);

/* perform tasks scheduled by interrupts */
void counter_try ();

#endif
