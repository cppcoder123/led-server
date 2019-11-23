//
//
//
#ifndef BASH_MSG_HPP
#define BASH_MSG_HPP

#include <memory>
#include <string>

namespace led_d
{
  class session_t;

  struct bash_msg_t
  {
    bash_msg_t () = delete;

    using session_ptr_t = std::shared_ptr<session_t>;
    bash_msg_t (const std::string &text, session_ptr_t session)
      : info (text),
        sender (session)
    {}
    ~bash_msg_t () {};

    std::string info;           // encoded msg
    session_ptr_t sender;       // ability to send reply
  };

} // namespace led_d

#endif
