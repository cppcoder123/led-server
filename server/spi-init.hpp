/*
 * Init level converter, enable level shifter
 */
#ifndef SPI_INIT_HPP
#define SPI_INIT_HPP

#include <gpiod.h>

namespace led_d
{

  class spi_init_t
  {
  public:

    spi_init_t ();
    ~spi_init_t ();

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
