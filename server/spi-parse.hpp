/*
 *
 */
#ifndef SPI_PARSE_HPP
#define SPI_PARSE_HPP

#include "type-def.hpp"

namespace led_d
{

  class spi_parse_t
  {
  public:
    spi_parse_t () = default;
    ~spi_parse_t () = default;

    bool push (char_t info, mcu_msg_t &msg);

  private:
    mcu_msg_t m_buf;
  };
  
} // namespace led_d

#endif
