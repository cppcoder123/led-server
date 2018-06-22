//
//
//
#ifndef LED_D_IDLE_MESSAGE_HPP
#define LED_D_IDLE_MESSAGE_HPP

#include "request.hpp"

namespace led_d
{

  struct idle_request_t : public core::request_t
  {
    idle_request_t ();
    ~idle_request_t () {}
  };

} // namespace led_d

#endif
