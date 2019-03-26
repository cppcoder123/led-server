/*
 * Switch on/off pcb led
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

void debug_init ();

void debug_switch (uint8_t val/*0 to switch off*/);

#endif
