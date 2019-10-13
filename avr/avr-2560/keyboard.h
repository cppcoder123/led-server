/*
 *
 */
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init ();

void keyboard_try ();

enum {
  KEYBOARD_OK,
  KEYBOARD_ERROR
};
uint8_t keyboard_status ();

#endif
