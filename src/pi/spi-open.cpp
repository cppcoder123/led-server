/*
 *
 */

#include <stdexcept>

#include "unix/log.hpp"

#include "spi-open.hpp"

namespace
{

  auto chip_name = "gpiochip0";

  auto enable_offset = 24;      // fixme: check

  auto confirm_offset = 12;     // check

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

    auto confirm_line = gpiod_chip_get_line (m_chip, confirm_offset);
    if (confirm_line == NULL)
      throw std::runtime_error ("spi-open: Failed to open confirm line");

    if (gpiod_line_request_rising_edge_events (confirm_line, consumer ()) != 0)
      throw std::runtime_error
        ("spi-open: Failed to request rising event on confirm line");

    log_t::info ("spi-open: Waiting for spi channel confirmation ...");

    // configure enable for output and set to 1
    if (gpiod_line_request_output (m_enable, consumer (), 1) != 0)
      throw std::runtime_error
        ("spi-open: Failed to configure enable for output");

    struct timespec spec;
    spec.tv_sec = 1;            // wait no more than 1 second
    spec.tv_nsec = 0;
    auto code = gpiod_line_event_wait (confirm_line, &spec);
    if (code == -1)
      throw std::runtime_error
        ("spi-open: Failed to start waiting for confirmation");
    else if (code == 0)
      throw std::runtime_error
        ("spi-open: Timed out while waiting for confirmation");
    else if (code == 1)
      log_t::info ("spi-open: Spi channel is confirmed!");
    else
      throw std::runtime_error
        ("spi-open: Unknown return code in \"event_wait\"");

    // Do we need to tell gpiod we are no longer interested in confirm events?
    gpiod_line_release (confirm_line);
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
