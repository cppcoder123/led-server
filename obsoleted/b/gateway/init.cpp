//
//
//
#include "clock.hpp"
#include "init.hpp"

namespace led_info_d
{

  void init (daemon_t &daemon)
  {
    static clock_t clock (daemon);
  }
  
} // namespace led_info_d
