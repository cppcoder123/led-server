/*
 *
 */
#ifndef POSTPONE_H
#define POSTPONE_H

#include "data-type.h"

void postpone_init ();

void postpone_try ();

void postpone_message (data_t *msg, uint8_t msg_size, data_t serial_id);

#endif
