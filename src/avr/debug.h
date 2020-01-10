/*
 * Switch on/off pcb led
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

void debug_0 (uint8_t domain, uint8_t key);
void debug_1 (uint8_t domain, uint8_t key, uint8_t value_1);
void debug_2 (uint8_t domain, uint8_t key, uint8_t value_1, uint8_t value_2);
void debug_3 (uint8_t domain, uint8_t key,
              uint8_t value_1, uint8_t value_2, uint8_t value_3);

#endif
