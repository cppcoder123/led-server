/*
 *
 */
#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>

void encode_msg_0 (uint8_t msg_id, uint8_t serial_id);

void encode_msg_1 (uint8_t msg_id, uint8_t serial_id, uint8_t payload_1);

void encode_msg_2 (uint8_t msg_id, uint8_t serial_id, uint8_t payload_1,
                   uint8_t payload_2);

void encode_msg_3 (uint8_t msg_id, uint8_t serial_id, uint8_t payload_1,
                   uint8_t payload_2, uint8_t payload_3);

void encode_msg_4 (uint8_t msg_id, uint8_t serial_id, uint8_t payload_1,
                   uint8_t payload_2, uint8_t payload_3, uint8_t payload_4);

void encode_msg_5 (uint8_t msg_id, uint8_t serial_id, uint8_t payload_1,
                   uint8_t payload_2, uint8_t payload_3, uint8_t payload_4,
                   uint8_t payload_5);

void encode_msg_8 (uint8_t msg_id, uint8_t serial_id, uint8_t payload_1,
                   uint8_t payload_2, uint8_t payload_3, uint8_t payload_4,
                   uint8_t payload_5, uint8_t payload_6, uint8_t payload_7,
                   uint8_t payload_8);

#endif
