//
//
//
#ifndef LED_D_MESSAGE_HPP
#define LED_D_MESSAGE_HPP

#include <string>

#include "session.hpp"

namespace led_d
{
  
  struct message_t
  {
    message_t () = delete;
    message_t (const std::string &text, session_ptr_t session)
      : info (text),
        sender (session)
    {}
    ~message_t () {};
    
    std::string info;           // encoded msg
    session_ptr_t sender;       // ability to send reply
  };
    
} // namespace led_d

#endif
