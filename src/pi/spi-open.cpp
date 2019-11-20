/*
 *
 */

#include <stdexcept>

#include "spi-open.hpp"

namespace
{

  auto chip_name = "gpiochip0";

  auto enable_offset = 22;      // fixme: check

} // anonymous

namespace led_d
{

  spi_open_t::spi_open_t ()
    : m_chip (NULL),
      m_enable (NULL)
  {
  }

  spi_open_t::~spi_open_t ()
  {
    stop ();
  }

  void spi_open_t::start ()
  {
    m_chip = gpiod_chip_open_by_name (chip_name);
    if (m_chip == NULL)
      throw std::runtime_error ("spi-open: Failed to open the chip");

    m_enable = gpiod_chip_get_line (m_chip, enable_offset);
    if (m_enable == NULL)
      throw std::runtime_error ("spi-open: Failed to open enable line");

    // configure enable for output and set to 1
    if (gpiod_line_request_output (m_enable, consumer (), 1) != 0)
      throw std::runtime_error ("spi-open: Failed to configure enable for output");
  }

  void spi_open_t::stop ()
  {
    if (m_enable) {
      gpiod_line_release (m_enable);
      m_enable = nullptr;
    }

    if (m_chip) {
      gpiod_chip_close (m_chip);
      m_chip = nullptr;
    }
  }
} // led_d
