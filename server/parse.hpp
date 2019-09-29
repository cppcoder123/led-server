/*
 *
 */
#ifndef PARSE_HPP
#define PARSE_HPP

#include "mcu-msg.hpp"

#include "unix/char-type.hpp"

namespace led_d
{

  class parse_t
  {
  public:
    parse_t () = default;
    ~parse_t () = default;

    // return true if complete msg is arrived,
    // unwrap-ed msg is assigned to 'msg'
    bool push (unix::char_t info, mcu_msg_t &msg);

  private:
    mcu_msg_t m_buf;
  };
  
} // namespace led_d

#endif
