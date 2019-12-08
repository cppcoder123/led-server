/*
 *
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

struct display_t
{
  uint8_t cs;                   /* chip-select, start of communication */
  uint8_t clk;                  /* clock signal */
  uint8_t data;                 /* data we want to transfer */
};


/* init internals & start matrix*/
void display_init (struct display_t *display, uint8_t cs, uint8_t clk, uint8_t data);

/* init led matrix */
void display_start (struct display_t *display);
void display_stop (struct display_t *display);

/*transfer data to led matrix*/
void display_data_start (struct display_t *display);
/*call 'display_column' 32 times, for each column*/
void display_data_column (struct display_t *display, uint8_t data);
void display_data_stop (struct display_t *display);

/*tweak brightness*/
void display_brightness (struct display_t *display, uint8_t value/*0-15*/);

#endif
