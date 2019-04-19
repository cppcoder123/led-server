/*
 *
 */
#ifndef ENCODE_H
#define ENCODE_H

#include "data-type.h"

void encode_msg_1 (data_t msg_id, data_t serial_id, data_t payload_1);

void encode_msg_2 (data_t msg_id, data_t serial_id,
                   data_t payload_1, data_t payload_2);

void encode_msg_3 (data_t msg_id, data_t serial_id,
                   data_t payload_1, data_t payload_2, data_t payload_3);

#endif
