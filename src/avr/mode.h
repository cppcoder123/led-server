/*
 *
 */
#ifndef MODE_H
#define MODE_H

#include <stdint.h>

void mode_init ();

enum {
  MODE_IDLE,
  MODE_MASTER,
  MODE_SLAVE,
};

void mode_set (uint8_t new_mode);

uint8_t mode_get ();

#endif
