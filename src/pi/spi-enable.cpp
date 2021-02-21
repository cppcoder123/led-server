/*
 *
 */

#include <stdexcept>

#include "util/log.hpp"

#include "spi-enable.hpp"

namespace
{
  auto chip_name = "gpiochip0";
  auto enable_offset = 27;      // check
  auto confirm_offset = 12;     // check
  auto reset_offset = 22;       // check
} // anonymous

namespace led_d
{

  spi_enable_t::spi_enable_t ()
    : m_chip (NULL),
      m_enable (NULL)
  {
  }

  spi_enable_t::~spi_enable_t ()
  {
    stop ();
  }

  void spi_enable_t::start ()
  {
    m_chip = gpiod_chip_open_by_name (chip_name);
    if (m_chip == NULL)
      throw std::runtime_error ("spi-enable: Failed to open the chip");

    // 1. set reset to high, otherwise avr will stuck
    auto reset = gpiod_chip_get_line (m_chip, reset_offset);
    if (reset == NULL)
      throw std::runtime_error ("spi-enable: Failed to open reset line");
    if (gpiod_line_request_output (reset, consumer (), 1) != 0)
      throw std::runtime_error ("spi-enable: Failed to set high level to RESET");
    // now we can close reset line, pi keeps its value
    gpiod_line_release (reset);

    // 2. enable spi channel
    m_enable = gpiod_chip_get_line (m_chip, enable_offset);
    if (m_enable == NULL)
      throw std::runtime_error ("spi-enable: Failed to open enable line");
    // configure enable for output and set to 1
    if (gpiod_line_request_output (m_enable, consumer (), 1) != 0)
      throw std::runtime_error
        ("spi-enable: Failed to configure enable for output");
    if (gpiod_line_set_value (m_enable, 1) != 0)
      throw std::runtime_error
        ("spi-enable: Failed to set enable line to 1");

    // 3. check confirm line. Is avr alive?
    auto confirm_line = gpiod_chip_get_line (m_chip, confirm_offset);
    if (confirm_line == NULL)
      log_t::info ("spi-enable: Failed to get confirm line");
    if (gpiod_line_request_input (confirm_line, consumer ()) != 0)
      log_t::info ("spi-enable: Failed to configure confirm line as input");
    auto status = gpiod_line_get_value (confirm_line);
    if (status != 1)
      throw std::runtime_error
        ("spi-enable: Spi confirm line has a bad value!");
    else
      log_t::info ("spi-enable: Spi confirm line is OK!");
    gpiod_line_release (confirm_line);
  }

  void spi_enable_t::stop ()
  {
    if (m_enable) {
      // set level shifter to Z state
      gpiod_line_set_value (m_enable, 0);
      gpiod_line_release (m_enable);
      m_enable = nullptr;
    }

    // if (m_reset) {
    //   gpiod_line_release (m_reset);
    //   m_reset = nullptr;
    // }

    if (m_chip) {
      gpiod_chip_close (m_chip);
      m_chip = nullptr;
    }
  }
} // led_d
