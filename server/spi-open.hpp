/*
 * Enable level converter (and spi)
 */
#ifndef SPI_OPEN_HPP
#define SPI_OPEN_HPP

#include <gpiod.h>

namespace led_d
{

  class spi_open_t
  {
  public:

    spi_open_t ();
    ~spi_open_t ();

    void start ();
    void stop ();

    static const char* consumer () {return "led-d";}

    gpiod_chip* chip () {return m_chip;}

  private:

    gpiod_chip *m_chip;

    gpiod_line *m_enable;
  };
  
} // led_d

#endif
