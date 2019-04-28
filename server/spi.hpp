//
// Spi device handling
//
#ifndef SPI_HPP
#define SPI_HPP

#include "type-def.hpp"

namespace led_d
{

  class spi_t
  {

  public:
    spi_t ();
    ~spi_t ();

    void start ();
    void stop ();

    void transfer (const mcu_msg_t &send, mcu_msg_t &receive);

  private:

    // clear spi channel as last step in 'start'
    void drain ();

    int m_device;               // unix device

  };

} // namespace led_d

#endif
