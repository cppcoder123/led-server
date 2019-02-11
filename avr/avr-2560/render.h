/*
 * render a symbol into pixel array
 */
#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>

enum {
  RENDER_0,
  RENDER_1,
  RENDER_2,
  RENDER_3,
  RENDER_4,
  RENDER_5,
  RENDER_6,
  RENDER_7,
  RENDER_8,
  RENDER_9,
  RENDER_SPACE,
  RENDER_COLON
};

void render_clear ();

/*convert 0-9 to RENDER_xxx, except space & colon*/
uint8_t render_id (uint8_t digit);

/*conevrt RENDER_XXX to stream of bytes and push them into flush*/
void render (uint8_t symbol);

void render_direct (uint8_t symbol, uint8_t times);


#endif
