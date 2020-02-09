/*
 *
 *
 *
 */

#ifndef POWER_H
#define POWER_H

#include <stdint.h>

void power_init ();

enum {
  POWER_UNKNOWN,                /* initial state */
  POWER_MASTER,                 /* pi is disconnected */
  POWER_SLAVE,                  /* pi is connected */
};

void power_set_mode (uint8_t mode);

uint8_t power_get_mode ();

#endif
