//
//
//
#ifndef LED_INFO_D_LOG_WRAPPER_HPP
#define LED_INFO_D_LOG_WRAPPER_HPP

#include "unix/log.hpp"

namespace led_info_d
{
  class log_wrapper_t
  {

  public:

    static void init (bool foreground, char *argv0);
    
  };

  typedef core::log_t log_t;
  
} // namespace led_info_d

#endif
