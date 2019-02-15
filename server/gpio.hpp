/*
 *
 */
#ifndef GPIO_HPP
#define GPIO_HPP

#include <gpiod.h>

namespace led_d
{

  class gpio_t
  {
  public:
    gpio_t ();
    ~gpio_t () {};

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