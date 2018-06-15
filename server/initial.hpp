//
//
//
#ifndef LED_D_INITIAL_HPP
#define LED_D_INITIAL_HPP

#include "serial.hpp"

namespace led_d
{

  class initial_t
  {

  public:
    initial_t () = default;
    ~initial_t () = default;

    void start (serial_t &serial);
  };
  
} // namespace led_d

#endif
