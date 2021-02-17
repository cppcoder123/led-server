/*
 * Calculate mcu id
 */
#ifndef MCU_ID_HPP
#define MCU_ID_HPP

#include <cstdint>

#include "const/constant.h"

namespace led_d
{

  class mcu_id
  {

  public:

    static uint8_t get ();
  };

  /*
   *----------------------------------------
   */
  inline uint8_t mcu_id::get ()
  {
    static uint8_t serial_id (0);
    if (++serial_id == SERIAL_ID_TO_IGNORE)
      ++serial_id;

    return serial_id;
  }

} // namespace led_d


#endif
