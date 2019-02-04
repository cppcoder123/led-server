/*
 *
 */
#ifndef MSG_HPP
#define MSG_HPP

#include "mcu/constant.h"

#include "type-def.hpp"

namespace led_d
{
  char_t msg_get_serial (const msg_t &msg);

  char_t msg_get_id (const msg_t &msg);

  // ----------------------------------------
  
  inline char_t msg_get_serial (const msg_t &msg)
  {
    if (msg.empty () == true)
      return SERIAL_ID_TO_IGNORE;

    return msg.front ();
  }

  inline char_t msg_get_id (const msg_t &msg)
  {
    if (msg.size () < 2)
      return MSG_ID_EMPTY;

    msg_t::const_iterator iter = msg.cbegin ();
    ++iter;

    return *iter;
  }
} // namespace led_d

#endif
