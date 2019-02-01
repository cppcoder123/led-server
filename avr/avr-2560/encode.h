/*
 *
 */
#ifndef ENCODE_H
#define ENCODE_H

#include "data-type.h"

uint8_t encode_msg_1 (data_t msg_id, data_t serial_id, data_t payload_1);

uint8_t encode_msg_2 (data_t msg_id, data_t serial_id,
                      data_t payload_1, data_t payload_2);

#endif
