/*
 *
 */
#ifndef ROTOR_H
#define ROTOR_H

#include <stdint.h>

void rotor_init ();

void rotor_try ();

/* id */
enum {
  ROTOR_1,
  ROTOR_2,
};

/* action */
enum {
  ROTOR_CLOCKWISE,
  ROTOR_COUNTER_CLOCKWISE,
  ROTOR_PUSH,
};

/* register callback to handle rotor events */
typedef void (*rotor_callback) (uint8_t id, uint8_t action);

void rotor_register (rotor_callback cb);

#endif
