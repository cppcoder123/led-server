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
    spi_parse_t ();
    ~spi_parse_t ();

    bool push (char_t info, msg_t &msg);
  };
  
} // namespace led_d

#endif
