/*
 *
 */
#ifndef ROTOR_H
#define ROTOR_H

/* #include <stdint.h> */

void rotor_init ();

void rotor_try ();

enum {
  /* knob ids */
  ROTOR_0,
  ROTOR_1,
  /* actions */
  ROTOR_CLOCKWISE,
  ROTOR_COUNTER_CLOCKWISE,
  ROTOR_PUSH,
};

#endif
