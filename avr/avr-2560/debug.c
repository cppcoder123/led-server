/*
 *
 */

/* #include <avr/io.h> */

#include "unix/constant.h"

#include "debug.h"
#include "encode.h"

/* #define LED_PORT PORTC6 */

/* void debug_init () */
/* { */
/*   DDRC |= (1 << LED_PORT); */
/*   /\*check*\/ */
/*   debug_switch (0); */
/* } */

/* void debug_switch (uint8_t state) */
/* { */
/*   if (state != 0) */
/*     PORTC |= (1 << LED_PORT); */
/*   else */
/*     PORTC &= ~(1 << LED_PORT); */
/* } */


void debug_0 (uint8_t domain, uint8_t key)
{
  encode_msg_2 (MSG_ID_DEBUG, SERIAL_ID_TO_IGNORE, domain, key);
}

void debug_1 (uint8_t domain, uint8_t key, uint8_t value_1)
{
  encode_msg_3 (MSG_ID_DEBUG, SERIAL_ID_TO_IGNORE, domain, key, value_1);
}

void debug_2 (uint8_t domain, uint8_t key, uint8_t value_1, uint8_t value_2)
{
  encode_msg_4 (MSG_ID_DEBUG,
                SERIAL_ID_TO_IGNORE, domain, key, value_1, value_2);
}
