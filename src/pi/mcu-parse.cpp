/*
 *
 */

#include "const/constant.h"

#include "util/log.hpp"

#include "mcu-decode.hpp"
#include "mcu-parse.hpp"

namespace led_d
{

  bool mcu_parse_t::push (uint8_t info, mcu_msg_t &msg)
  {
    if (m_buf.empty () == true) {
      if (info == SPI_WRITE_UNDERFLOW)
        return false;
      if (info == SPI_READ_OVERFLOW) {
        log_t::buffer_t buf;
        buf << "spi: Read buffer is full";
        log_t::error (buf);
        return false;
      }
    }

    m_buf.push_back (info);
    if (mcu::decode::unwrap (m_buf) == false)
      return false;

    msg = m_buf;
    m_buf.clear ();

    return true;
  }

} // namespace led_d
