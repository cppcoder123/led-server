/*
 * Calculate serial id
 */
#ifndef SERIAL_ID_HPP
#define SERIAL_ID_HPP

#include "mcu/constant.h"

#include "type-def.hpp"

namespace led_d
{

  class serial
  {

  public:

    static char_t get ();
  };

  /*
   *----------------------------------------
   */
  inline char_t serial::get ()
  {
    static char_t serial_id (0);
    if (++serial_id == SERIAL_ID_TO_IGNORE)
      ++serial_id;

    return serial_id;
  }

} // namespace led_d


#endif