/*
 *
 */

#include "mcu/constant.h"

#include "mcu-decode.hpp"
#include "spi-parse.hpp"

namespace led_d
{

  bool spi_parse_t::push (char_t info, msg_t &msg)
  {
    if (info == SPI_WRITE_UNDERFLOW)
      return false;

    m_buf.push_back (info);
    if (mcu::decode::unwrap (m_buf) == false)
      return false;

    msg = m_buf;
    return true;
  }

} // namespace led_d
