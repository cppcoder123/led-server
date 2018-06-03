/*
 *
 *
 *
 */
#ifndef CODEC_H
#define CODEC_H

#include <stdint.h>

void codec_init ();

void codec_encode_0 (uint8_t msg_id, uint16_t msg_serial_id);
void codec_encode_1 (uint8_t msg_id, uint16_t msg_serial_id, uint8_t data);

/*It returns zero if failed, src should point to EYE_CATCHER*/
uint8_t codec_decode_header (volatile uint8_t *src,
                             uint16_t *msg_size,
                             uint8_t *msg_id,
                             uint16_t *msg_serial_id);

#endif
