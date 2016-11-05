//
//
//
#ifndef LED_D_IDLE_MESSAGE_HPP
#define LED_D_IDLE_MESSAGE_HPP

#include "libled/request.hpp"

namespace led_d
{

  struct idle_request_t : public libled::request_t
  {
    idle_request_t ();
    ~idle_request_t () {}
  };

  //
  //
  //
  
  inline idle_request_t::idle_request_t ()
  {
    action = request_t::action_insert;
    info = "No message to display";
  }
  
} // namespace led_d

#endif
