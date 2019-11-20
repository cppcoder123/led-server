/*
 *
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

/* init internals & start matrix*/
void display_init ();

/* init led matrix */
void display_start ();
void display_stop ();

/*transfer data to led matrix*/
void display_data_start ();
/*call 'display_column' 32 times, for each column*/
void display_data_column (uint8_t data);
void display_data_stop ();

/*tweak brightness*/
void display_brightness (uint8_t value/*0-15*/);

#endif
