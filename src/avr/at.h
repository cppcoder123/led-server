/*
 * Invoke callback once.
 * This is a functionality similar to Unix 'at' command.
 */
#ifndef AT_H
#define AT_H

#include <stdint.h>

void at_init ();

enum {
  AT_BOUNCE_0,
  AT_BOUNCE_1,
  AT_MENU,
  AT_POWER,
  AT_MAX,                       /* keep it last */
};

typedef void (*at_callback) ();

/*
 * 'delay' delay in seconds
 */
void at_schedule (uint8_t id, uint8_t delay, at_callback cb);

void at_postpone (uint8_t id);

void at_cancel (uint8_t id);

uint8_t at_empty (uint8_t id);

#endif
