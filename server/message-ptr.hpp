//
//
//
#ifndef LED_D_MESSAGE_PTR_HPP
#define LED_D_MESSAGE_PTR_HPP

#include <memory>

#include "message.hpp"

namespace led_d
{

  //typedef std::shared_ptr <message_t> message_ptr_t;
  using message_ptr_t = std::unique_ptr <message_t>;
  
} // namespace led_d

#endif
