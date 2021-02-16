/*
 *
 */
#ifndef MODE_H
#define MODE_H

#include <stdint.h>

void mode_init ();

enum {
  MODE_MENU,
  MODE_WATCH,
  MODE_RADIO,
};

void mode_set (uint8_t new_mode);

uint8_t mode_get ();

uint8_t mode_is_connnected ();

#endif
