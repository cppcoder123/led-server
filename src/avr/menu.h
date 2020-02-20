/*
 *
 */
#ifndef MENU_H
#define MENU_H

void menu_init ();

/* store parameter value */
uint8_t menu_parameter_value (uint8_t parameter,
                              uint8_t value, uint8_t min, uint8_t max);

#endif
