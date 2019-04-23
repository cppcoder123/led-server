/*
 *
 */
#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>

void encode_msg_1 (uint8_t msg_id, uint8_t serial_id, uint8_t payload_1);

void encode_msg_2 (uint8_t msg_id, uint8_t serial_id,
                   uint8_t payload_1, uint8_t payload_2);

void encode_msg_3 (uint8_t msg_id, uint8_t serial_id,
                   uint8_t payload_1, uint8_t payload_2, uint8_t payload_3);

#endif
