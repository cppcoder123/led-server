/*
 *
 */
#ifndef SPI_GPIO_HPP
#define SPI_GPIO_HPP

namespace led_d
{

  class spi_gpio_t
  {
  public:
    spi_gpio_t ();
    ~spi_gpio_t ();

    void start ();
    void stop ();

    bool is_irq_raised ();
  };
  
} // namespace led_d

#endif
