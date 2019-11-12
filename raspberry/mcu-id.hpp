/*
 * Calculate mcu id
 */
#ifndef MCU_ID_HPP
#define MCU_ID_HPP

#include "unix/char-type.hpp"
#include "unix/constant.h"

namespace led_d
{

  class mcu_id
  {

  public:

    static unix::char_t get ();
  };

  /*
   *----------------------------------------
   */
  inline unix::char_t mcu_id::get ()
  {
    static unix::char_t serial_id (0);
    if (++serial_id == SERIAL_ID_TO_IGNORE)
      ++serial_id;

    return serial_id;
  }

} // namespace led_d


#endif
