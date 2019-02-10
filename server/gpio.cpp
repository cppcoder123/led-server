/*
 *
 */


#include <stdexcept>

#include "log-wrapper.hpp"
#include "gpio.hpp"

namespace
{

  auto chip_name = "gpiochip0";

  auto enable_offset = 5;       // fixme: check
  auto irq_offset = 26;         // fixme: check

  auto consumer = "led-d";

} // namespace

namespace led_d
{
  gpio_t::gpio_t ()
    : m_chip (NULL),
      m_enable (NULL),
      m_irq (NULL)
  {
  }
  
  void gpio_t::start ()
  {
    m_chip = gpiod_chip_open_by_name (chip_name);
    if (m_chip == NULL)
      throw std::runtime_error ("gpio: Failed to open the chip");

    m_enable = gpiod_chip_get_line (m_chip, enable_offset);
    if (m_enable == NULL)
      throw std::runtime_error ("gpio: Failed to open enable line");

    m_irq = gpiod_chip_get_line (m_chip, irq_offset);
    if (m_irq == NULL)
      throw std::runtime_error ("gpio: Failed to open irq line");

    // configure enable for output and set to 1
    if (gpiod_line_request_output (m_enable, consumer, 1) != 0)
      throw std::runtime_error ("gpio: Failed to configure enable for output");
    if (gpiod_line_request_input (m_irq, consumer) != 0)
      throw std::runtime_error ("gpio: Failed to configure irq for input");
  }

  void gpio_t::stop ()
  {
    if (m_enable)
      gpiod_line_release (m_enable);
    if (m_irq)
      gpiod_line_release (m_irq);

    if (m_chip)
      gpiod_chip_close (m_chip);
  }

  bool gpio_t::is_irq_raised ()
  {
    int res = gpiod_line_get_value (m_irq);
    if (res == 0)
      return false;
    if (res == 1)
      return true;

    log_t::buffer_t buf;
    buf << "gpio: Error while accessing gpio line";
    log_t::error (buf);
    
    return false;
  }
  
} // namespace led_d
