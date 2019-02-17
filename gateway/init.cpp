//
//
//
#include "clock.hpp"
#include "daemon.hpp"
#include "init.hpp"

namespace led_info_d
{

  void init_t::init (daemon_t &daemon)
  {
    // callback_t::info_t info_callback
    //   = std::bind (&daemon_t::info, &daemon,
    //                std::placeholders::_1, std::placeholders::_2);

    // callback_t::schedule_t schedule_callback
    //   = std::bind (&daemon_t::schedule, &daemon,
    //                std::placeholders::_1, std::placeholders::_2);
    
    static clock_t clock (daemon);
  }
  
} // namespace led_info_d
