//
//
//
#ifndef LED_D_LOG_WRAPPER_HPP
#define LED_D_LOG_WRAPPER_HPP

#include "unix/log.hpp"


namespace led_d
{
  class log_wrapper_t
  {

  public:

    static void init (bool foreground, char *argv0);
  };

  using log_t = unix::log_t;
  
  
} // namespace led_d

#endif
