//
//
//
#ifndef LED_INFO_D_LOG_WRAPPER_HPP
#define LED_INFO_D_LOG_WRAPPER_HPP

#include "libled/log.hpp"

namespace led_info_d
{
  class log_wrapper_t
  {

  public:

    static void init (bool foreground, char *argv0);
    
  };

  typedef libled::log_t log_t;
  
} // namespace led_info_d

#endif
