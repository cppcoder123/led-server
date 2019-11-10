/*
 * Switch on/off pcb led
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

/* debug domain */
enum {
  DEBUG_KEY_BOARD,
};

/* debug key */
enum {
  DEBUG_0,
  DEBUG_1,
  DEBUG_2,
  DEBUG_3,
  DEBUG_4,
  DEBUG_5,
  DEBUG_6,
  DEBUG_7,
  DEBUG_8,
  DEBUG_9,
  DEBUG_10,
  DEBUG_11,
  DEBUG_12,
  DEBUG_13,
  DEBUG_14,
  DEBUG_15,
  DEBUG_16,
  DEBUG_17,
  DEBUG_18,
  DEBUG_19,
  DEBUG_20,
};

/* void debug_init (); */

void debug_0 (uint8_t domain, uint8_t key);
void debug_1 (uint8_t domain, uint8_t key, uint8_t value_1);
void debug_2 (uint8_t domain, uint8_t key, uint8_t value_1, uint8_t value_2);

#endif
