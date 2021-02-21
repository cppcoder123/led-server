/*
 * Enable level converter (and spi)
 */
#ifndef SPI_ENABLE_HPP
#define SPI_ENABLE_HPP

#include <gpiod.h>

namespace led_d
{

  class spi_enable_t
  {
  public:

    spi_enable_t ();
    ~spi_enable_t ();

    void start ();
    void stop ();

    static const char* consumer () {return "led-d";}

    gpiod_chip* chip () {return m_chip;}

  private:

    gpiod_chip *m_chip;

    gpiod_line *m_enable;
    // gpiod_line *m_reset;
  };
  
} // led_d

#endif
