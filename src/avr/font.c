/*
 *
 */

#include <string.h>

#include "font.h"

#define SYMBOL_SIZE 5

static uint8_t fill_matrix(uint8_t matrix[], uint8_t symbol_id)
{
  const uint8_t *needed_symbol = 0;

  switch(symbol_id) {
  case FONT_0:
    {
      static const uint8_t symbol[] = {0x3E, 0x51, 0x49, 0x45, 0x3E};
      needed_symbol = symbol;
      break;
    }
  case FONT_1:
    {
      static const uint8_t symbol[] = {0x00, 0x42, 0x7F, 0x40, 0x00};
      needed_symbol = symbol;
      break;
    }
  case FONT_2:
    {
      static const uint8_t symbol[] = {0x42, 0x61, 0x51, 0x49, 0x46};
      needed_symbol = symbol;
      break;
    }
  case FONT_3:
    {
      static const uint8_t symbol[] = {0x21, 0x41, 0x45, 0x4B, 0x31};
      needed_symbol = symbol;
      break;
    }
  case FONT_4:
    {
      static const uint8_t symbol[] = {0x18, 0x14, 0x12, 0x7F, 0x10};
      needed_symbol = symbol;
      break;
    }
  case FONT_5:
    {
      static const uint8_t symbol[] = {0x27, 0x45, 0x45, 0x45, 0x39};
      needed_symbol = symbol;
      break;
    }
  case FONT_6:
    {
      static const uint8_t symbol[] = {0x3C, 0x4A, 0x49, 0x49, 0x30};
      needed_symbol = symbol;
      break;
    }
  case FONT_7:
    {
      static const uint8_t symbol[] = {0x01, 0x71, 0x09, 0x05, 0x03};
      needed_symbol = symbol;
      break;
    }
  case FONT_8:
    {
      static const uint8_t symbol[] = {0x36, 0x49, 0x49, 0x49, 0x36};
      needed_symbol = symbol;
      break;
    }
  case FONT_9:
    {
      static const uint8_t symbol[] = {0x06, 0x49, 0x49, 0x29, 0x1E};
      needed_symbol = symbol;
      break;
    }
  case FONT_A:
    {
      static const uint8_t symbol[] = {0x7E, 0x11, 0x11, 0x11, 0x7E};
      needed_symbol = symbol;
      break;
    }
  case FONT_B:
    {
      static const uint8_t symbol[] = {0x7F, 0x49, 0x49, 0x49, 0x36};
      needed_symbol = symbol;
      break;
    }
  case FONT_C:
    {
      static const uint8_t symbol[] = {0x3E, 0x41, 0x41, 0x41, 0x22};
      needed_symbol = symbol;
      break;
    }
  case FONT_D:
    {
      static const uint8_t symbol[] = {0x7F, 0x41, 0x41, 0x22, 0x1C};
      needed_symbol = symbol;
      break;
    }
  case FONT_E:
    {
      static const uint8_t symbol[] = {0x7F, 0x49, 0x49, 0x49, 0x41};
      needed_symbol = symbol;
      break;
    }
  case FONT_H:
    {
      static const uint8_t symbol[] = {0x7F, 0x08, 0x08, 0x08, 0x7F};
      needed_symbol = symbol;
      break;
    }
  case FONT_M:
    {
      static const uint8_t symbol[] = {0x7F, 0x02, 0x04, 0x02, 0x7F};
      needed_symbol = symbol;
      break;
    }
  case FONT_O:
    {
      static const uint8_t symbol[] = {0x3E, 0x41, 0x41, 0x41, 0x3E};
      needed_symbol = symbol;
      break;
    }
  case FONT_T:
    {
      static const uint8_t symbol[] = {0x01, 0x01, 0x7F, 0x01, 0x01};
      needed_symbol = symbol;
      break;
    }
  case FONT_V:
    {
      static const uint8_t symbol[] = {0x1F, 0x20, 0x40, 0x20, 0x1F};
      needed_symbol = symbol;
      break;
    }
  case FONT_X:
    {
      static const uint8_t symbol[] = {0x63, 0x14, 0x08, 0x14, 0x63};
      needed_symbol = symbol;
      break;
    }
  case FONT_Y:
    {
      static const uint8_t symbol[] = {0x03, 0x04, 0x78, 0x04, 0x03};
      needed_symbol = symbol;
      break;
    }
  case FONT_a:
    {
      static const uint8_t symbol[] = {0x20, 0x54, 0x54, 0x54, 0x78};
      needed_symbol = symbol;
      break;
    }
  case FONT_b:
    {
      static const uint8_t symbol[] = {0x7F, 0x48, 0x44, 0x44, 0x38};
      needed_symbol = symbol;
      break;
    }
  case FONT_c:
    {
      static const uint8_t symbol[] = {0x38, 0x44, 0x44, 0x44, 0x20};
      needed_symbol = symbol;
      break;
    }
  case FONT_e:
    {
      static const uint8_t symbol[] = {0x38, 0x54, 0x54, 0x54, 0x18};
      needed_symbol = symbol;
      break;
    }
  case FONT_f:
    {
      static const uint8_t symbol[] = {0x08, 0x7E, 0x09, 0x01, 0x02};
      needed_symbol = symbol;
      break;
    }
  case FONT_g:
    {
      static const uint8_t symbol[] = {0x08, 0x14, 0x54, 0x54, 0x3C};
      needed_symbol = symbol;
      break;
    }
  case FONT_h:
    {
      static const uint8_t symbol[] = {0x7F, 0x08, 0x04, 0x04, 0x78};
      needed_symbol = symbol;
      break;
    }
  case FONT_i:
    {
      static const uint8_t symbol[] = {0x00, 0x44, 0x7D, 0x40, 0x00};
      needed_symbol = symbol;
      break;
    }
  case FONT_k:
    {
      static const uint8_t symbol[] = {0x00, 0x7F, 0x10, 0x28, 0x44};
      needed_symbol = symbol;
      break;
    }
  case FONT_l:
    {
      static const uint8_t symbol[] = {0x00, 0x41, 0x7F, 0x40, 0x00};
      needed_symbol = symbol;
      break;
    }
  case FONT_m:
    {
      static const uint8_t symbol[] = {0x7C, 0x04, 0x18, 0x04, 0x78};
      needed_symbol = symbol;
      break;
    }
  case FONT_n:
    {
      static const uint8_t symbol[] = {0x7C, 0x08, 0x04, 0x04, 0x78};
      needed_symbol = symbol;
      break;
    }
  case FONT_o:
    {
      static const uint8_t symbol[] = {0x38, 0x44, 0x44, 0x44, 0x38};
      needed_symbol = symbol;
      break;
    }
  case FONT_r:
    {
      static const uint8_t symbol[] = {0x7C, 0x08, 0x04, 0x04, 0x08};
      needed_symbol = symbol;
      break;
    }
  case FONT_s:
    {
      static const uint8_t symbol[] = {0x48, 0x54, 0x54, 0x54, 0x20};
      needed_symbol = symbol;
      break;
    }
  case FONT_AT:
    {
      static const uint8_t symbol[] = {0x32, 0x49, 0x79, 0x41, 0x3E};
      needed_symbol = symbol;
      break;
    }
  case FONT_COLON:
    {
      static const uint8_t symbol[] = {0x00, 0x36, 0x36, 0x00, 0x00};
      needed_symbol = symbol;
      break;
    }
  case FONT_EQUAL:
    {
      static const uint8_t symbol[] = {0x14, 0x14, 0x14, 0x14, 0x14};
      needed_symbol = symbol;
      break;
    }
  case FONT_MINUS:
    {
      static const uint8_t symbol[] = {0x08, 0x08, 0x08, 0x08, 0x08};
      needed_symbol = symbol;
      break;
    }
  case FONT_MORE:
    {
      static const uint8_t symbol[] = {0x41, 0x22, 0x14, 0x08, 0x00};
      needed_symbol = symbol;
      break;
    }
  case FONT_PLUS:
    {
      static const uint8_t symbol[] = {0x08, 0x08, 0x3E, 0x08, 0x08};
      needed_symbol = symbol;
      break;
    }
  case FONT_SPACE:
    {
      static const uint8_t symbol[] = {0x00, 0x00, 0x00, 0x00, 0x00};
      needed_symbol = symbol;
      break;
    }
  default:
    return 0;
  }

  memcpy(matrix, needed_symbol, SYMBOL_SIZE);

  return 1;
}

uint8_t font_add_symbol(uint8_t symbol, uint8_t *buffer,
                        uint8_t *position, uint8_t position_limit)
{
  if ((*position + SYMBOL_SIZE >= position_limit)
      || (symbol >= FONT_MAX_SYMBOL))
    return 0;

  uint8_t matrix[SYMBOL_SIZE];
  if (fill_matrix(matrix, symbol) == 0)
    return 0;

  for (uint8_t i = 0; i < SYMBOL_SIZE; ++i)
    *(buffer + (*position)++) = matrix[i];

  return 1;
}
