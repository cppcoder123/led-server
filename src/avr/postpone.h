/*
 * This is one message spi-write buffer.
 *
 * All messages can be split into 2 parts with SERIAL_ID_TO_IGNORE
 * and serial id that can't be ignored. So, messages that have to
 * be ignored can be send without delay. Messages with meaningfull
 * id can be send to pi only we have enough space in avr queues:
 * spi-write-queue, spi-read-queue, flush-queue
 *
 */
#ifndef POSTPONE_H
#define POSTPONE_H

#include <stdint.h>

void postpone_init ();

void postpone_try ();

void postpone_message (uint8_t *msg, uint8_t msg_size, uint8_t serial_id);

#endif
