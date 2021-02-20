//
// Spi device handling
//
#ifndef SPI_DEV_HPP
#define SPI_DEV_HPP

#include "mcu-msg.hpp"

namespace led_d
{

  class spi_dev_t
  {

  public:

    spi_dev_t ();
    ~spi_dev_t ();

    void start ();
    void stop ();

    void transfer (const mcu_msg_t &send, mcu_msg_t &receive);

  private:

    // clear spi channel as last step in 'start'
    // void drain ();

    int m_device;               // unix device

  };

} // namespace led_d

#endif
