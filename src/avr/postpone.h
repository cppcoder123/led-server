/*
 * fixme: Remove this module
 */
#ifndef POSTPONE_H
#define POSTPONE_H

#include <stdint.h>

void postpone_init ();

void postpone_try ();

void postpone_message (uint8_t *msg, uint8_t msg_size, uint8_t serial_id);

#endif
