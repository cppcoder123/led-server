/*
 *
 */
#ifndef MENU_H
#define MENU_H

#include <stdint.h>

void menu_init ();

void menu_handle_rotor (uint8_t rotor_id, uint8_t rotor_action);

/* keep parameter range */
uint8_t menu_parameter_value (uint8_t parameter,
                              uint8_t value, uint8_t min, uint8_t max);

/* parameter name is ready for transfer*/
uint8_t menu_parameter_name (uint8_t parameter, uint8_t id);

#endif
