/*
 *
 */

#ifndef FONT_HEADER_H
#define FONT_HEADER_H

#include <stdint.h>

#include "buf.h"

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
  FONT_A,
  FONT_B,
  FONT_C,
  FONT_D,
  FONT_E,
  FONT_H,
  FONT_L,
  FONT_M,
  FONT_N,
  FONT_O,
  FONT_P,
  FONT_R,
  FONT_S,
  FONT_T,
  FONT_V,
  FONT_X,
  FONT_Y,
  FONT_a,
  FONT_b,
  FONT_c,
  FONT_d,
  FONT_e,
  FONT_f,
  FONT_g,
  FONT_h,
  FONT_i,
  FONT_k,
  FONT_l,
  FONT_m,
  FONT_n,
  FONT_o,
  FONT_p,
  FONT_r,
  FONT_s,
  FONT_t,
  FONT_u,
  FONT_v,
  FONT_x,
  FONT_y,
  FONT_AT,                    /* "21:10 @07:00" => 60 bytes, less than 64*/
  FONT_COLON,
  FONT_EQUAL,
  FONT_MINUS,
  FONT_MORE,
  FONT_PLUS,
  FONT_SPACE,
  FONT_STAR,
  FONT_MAX_SYMBOL,            /* Keep last */
};

/* return 0 in case of failure */
uint8_t font_add_symbol(uint8_t symbol, struct buf_t *buf);

#endif
