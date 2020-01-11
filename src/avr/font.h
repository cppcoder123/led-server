/*
 *
 */

#ifndef FONT_H
#define FONT_H

#include <stdint.h>

enum {
  FONT_0,
  FONT_1,
  FONT_2,
  FONT_3,
  FONT_4,
  FONT_5,
  FONT_6,
  FONT_7,
  FONT_8,
  FONT_9,
  FONT_AT,                    /* "21:10 @07:00" => 60 bytes, less than 64*/
  FONT_COLON,
  FONT_EQUAL,
  FONT_MORE,
  FONT_SPACE,
  FONT_X,
  FONT_Y,
  FONT_MAX_SYMBOL,            /* Keep last */
};

/* return 0 in case of failure */
uint8_t font_add_symbol(uint8_t symbol, uint8_t buffer[],
                        uint8_t *position, uint8_t limit_position);

#endif
