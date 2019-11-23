/*
 *
 */
#ifndef MCU_PARSE_HPP
#define MCU_PARSE_HPP

#include <cstdint>

#include "mcu-msg.hpp"

namespace led_d
{

  class mcu_parse_t
  {
  public:
    mcu_parse_t () = default;
    ~mcu_parse_t () = default;

    // return true if complete msg is arrived,
    // unwrap-ed msg is assigned to 'msg'
    bool push (uint8_t info, mcu_msg_t &msg);

  private:
    mcu_msg_t m_buf;
  };

} // namespace led_d

#endif
