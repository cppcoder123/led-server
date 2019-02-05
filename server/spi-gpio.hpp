/*
 *
 */
#ifndef SPI_GPIO_HPP
#define SPI_GPIO_HPP

#include <gpiod.h>

namespace led_d
{

  class spi_gpio_t
  {
  public:
    spi_gpio_t ();
    ~spi_gpio_t () {};

    void start ();
    void stop ();

    bool is_irq_raised ();

  private:

    gpiod_chip *m_chip;

    gpiod_line *m_enable;
    gpiod_line *m_irq;
    
  };
  
} // namespace led_d

#endif
