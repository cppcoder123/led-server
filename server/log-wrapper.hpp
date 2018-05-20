//
//
//
#ifndef LED_D_LOG_WRAPPER_HPP
#define LED_D_LOG_WRAPPER_HPP

#include "log.hpp"


namespace led_d
{
  class log_wrapper_t
  {

  public:

    static void init (bool foreground, char *argv0);
  };

  typedef core::log_t log_t;
  
  
} // namespace led_d

#endif
