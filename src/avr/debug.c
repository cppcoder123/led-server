/*
 *
 */

#include "unix/constant.h"

#include "debug.h"
#include "encode.h"
#include "mode.h"

void debug_0 (uint8_t domain, uint8_t key)
{
  if (mode_is_connnected () != 0)
    encode_msg_2 (MSG_ID_DEBUG, SERIAL_ID_TO_IGNORE, domain, key);
}

void debug_1 (uint8_t domain, uint8_t key, uint8_t value_1)
{
  if (mode_is_connnected () != 0)
    encode_msg_3 (MSG_ID_DEBUG, SERIAL_ID_TO_IGNORE, domain, key, value_1);
}

void debug_2 (uint8_t domain, uint8_t key, uint8_t value_1, uint8_t value_2)
{
  if (mode_is_connnected () != 0)
    encode_msg_4 (MSG_ID_DEBUG,
                  SERIAL_ID_TO_IGNORE, domain, key, value_1, value_2);
}

void debug_3 (uint8_t domain, uint8_t key,
              uint8_t value_1, uint8_t value_2, uint8_t value_3)
{
  if (mode_is_connnected () != 0)
    encode_msg_5 (MSG_ID_DEBUG,
                  SERIAL_ID_TO_IGNORE, domain, key, value_1, value_2, value_3);
}

void debug_4 (uint8_t domain, uint8_t key, uint8_t value_1,
              uint8_t value_2, uint8_t value_3, uint8_t value_4)
{
  if (mode_is_connnected () != 0)
    encode_msg_6 (MSG_ID_DEBUG,
                  SERIAL_ID_TO_IGNORE, domain, key,
                  value_1, value_2, value_3, value_4);
}
