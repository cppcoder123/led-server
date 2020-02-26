/*
 *
 */
#ifndef MENU_H
#define MENU_H

#include <stdint.h>

void menu_init ();

void menu_handle_rotor (uint8_t rotor_id, uint8_t rotor_action);

/* store parameter value */
uint8_t menu_parameter_value (uint8_t parameter,
                              uint8_t value, uint8_t min, uint8_t max);

#endif
