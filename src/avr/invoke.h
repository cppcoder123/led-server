/*
 * Invoke callbacks on time basis.
 *
 * This is an utility to share the only timer
 * to invoke different functions
 */

#ifndef INVOKE_H
#define INVOKE_H

#include <stdint.h>

void invoke_init ();

void invoke_try ();

typedef void (*invoke_callback) ();

enum {
  INVOKE_ID_BUZZ,
  /* INVOKE_ID_POLL, */
  /* INVOKE_ID_SPI, */
  INVOKE_ID_FLUSH,
  INVOKE_ID_POWER,
  INVOKE_ID_SYNC,
  INVOKE_ID_MAX,                /* Keep last */
};

uint8_t invoke_enable (uint8_t id,
                       uint8_t factor, invoke_callback callback);
uint8_t invoke_disable (uint8_t id);

#endif
