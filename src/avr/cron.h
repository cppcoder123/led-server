/*
 * Invoke callbacks on time basis.
 *
 * This is an utility to share the only timer
 * to invoke different functions
 */

#ifndef CRON_H
#define CRON_H

#include <stdint.h>

void cron_init ();

void cron_try ();

typedef void (*cron_callback) ();

enum {
  CRON_ID_AT,
  CRON_ID_BUZZ,
  CRON_ID_FAN,
  CRON_ID_CLOCK,
  CRON_ID_FLUSH,
  CRON_ID_HEARTBEAT,
  /* CRON_ID_ROTOR, */
  CRON_ID_MAX,                /* Keep last */
};

/* factor is 0.02 sec */
uint8_t cron_enable (uint8_t id, uint8_t factor, cron_callback callback);
uint8_t cron_disable (uint8_t id);

#endif
