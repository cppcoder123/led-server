/*
 *
 */

#include "data-type.h"
#include "flush.h"
#include "render.h"

#define LETTER_SIZE 5

static data_t slim_font[] = {
  0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
  0x00, 0x42, 0x7F, 0x40, 0x00, // 1
  0x42, 0x61, 0x51, 0x49, 0x46, // 2
  0x21, 0x41, 0x45, 0x4B, 0x31, // 3
  0x18, 0x14, 0x12, 0x7F, 0x10, // 4
  0x27, 0x45, 0x45, 0x45, 0x39, // 5
  0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
  0x01, 0x71, 0x09, 0x05, 0x03, // 7
  0x36, 0x49, 0x49, 0x49, 0x36, // 8
  0x06, 0x49, 0x49, 0x29, 0x1E, // 9
  0x00, 0x00, 0x00, 0x00, 0x00, // (space)
  0x00, 0x36, 0x36, 0x00, 0x00  // :
};

void render_clear ()
{
  flush_init ();
}

uint8_t render_id (uint8_t digit)
{
  switch (digit) {
  case 0:
    return RENDER_0;
  case 1:
    return RENDER_1;
  case 2:
    return RENDER_2;
  case 3:
    return RENDER_3;
  case 4:
    return RENDER_4;
  case 5:
    return RENDER_5;
  case 6:
    return RENDER_6;
  case 7:
    return RENDER_7;
  case 8:
    return RENDER_8;
  case 9:
    return RENDER_9;
  default:
    return RENDER_COLON;
  }

  return RENDER_COLON;
}

void render (uint8_t id)
{
  for (uint8_t j = 0; j < LETTER_SIZE; ++j)
    flush_push_mono (slim_font[id * LETTER_SIZE + j]);
}

void render_direct (uint8_t symbol, uint8_t times)
{
  for (uint8_t i = 0; i < times; ++i)
    flush_push_mono (symbol);
}
