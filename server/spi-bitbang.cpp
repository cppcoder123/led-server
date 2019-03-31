//
//
//

#include <chrono>
#include <thread>

#include "gpio.hpp"
#include "log-wrapper.hpp"
#include "spi-bitbang.hpp"

namespace led_d
{

  spi_bitbang_t::spi_bitbang_t ()
    : m_ss (NULL),
      m_mosi (NULL),
      m_clk (NULL),
      m_miso (NULL)
  {
  }

  void spi_bitbang_t::start (gpiod_chip *chip)
  {
    if (chip == NULL)
      throw std::runtime_error ("bitbang: Empty chip");

    if ((m_ss = gpiod_chip_get_line (chip, m_ss_offset)) == NULL)
      throw std::runtime_error ("bitbang: Failed to get ss line");
    if ((m_mosi = gpiod_chip_get_line (chip, m_mosi_offset)) == NULL)
      throw std::runtime_error ("bitbang: Failed to get mosi line");
    if ((m_clk = gpiod_chip_get_line (chip, m_clk_offset)) == NULL)
      throw std::runtime_error ("bitbang: Failed to get clk line");
    if ((m_miso = gpiod_chip_get_line (chip, m_miso_offset)) == NULL)
      throw std::runtime_error ("bitbang: Failed to get miso line");

    if (gpiod_line_request_output (m_ss, gpio_t::get_consumer (), high) != 0)
      throw std::runtime_error ("bitbang: Failed to request ss");
    if (gpiod_line_request_output (m_mosi, gpio_t::get_consumer (), low) != 0)
      throw std::runtime_error ("bitbang: Failed to request mosi");
    if (gpiod_line_request_output (m_clk, gpio_t::get_consumer (), low) != 0)
      throw std::runtime_error ("bitbang: Failed to request clk");
    if (gpiod_line_request_input (m_miso, gpio_t::get_consumer ()) != 0)
      throw std::runtime_error ("bitbang: Failed to request miso");
  }

  void spi_bitbang_t::stop ()
  {
    auto release = [](gpiod_line *&line)
      {
        if (line == NULL)
          return;

        gpiod_line_release (line);
        line = NULL;
      };

    release (m_ss);
    release (m_mosi);
    release (m_clk);
    release (m_miso);
  }

  void spi_bitbang_t::transfer (const mcu_msg_t &out, mcu_msg_t &in)
  {
    char_t in_char;
    for (char_t out_char : out) {
      transfer_char (out_char, in_char);
      in.push_back (in_char);
    }
  }

  void spi_bitbang_t::transfer_char (char_t out, char_t &in)
  {
    static const constexpr std::chrono::microseconds delay (50);

    write_line (m_ss, low);
    std::this_thread::sleep_for (delay);

    // sample on rising edge, msb first
    char_t mask = (1 << 7);
    in = 0;

    for (int i = 0; i < 8; ++i) {
      // setup
      write_line (m_mosi, ((out & mask) != 0) ? high : low);
      std::this_thread::sleep_for (delay);
      // processing edge
      write_line (m_clk, high);
      std::this_thread::sleep_for (delay);
      // slave read
      int read = read_line (m_miso);
      //
      write_line (m_clk, low);
      std::this_thread::sleep_for (delay);
      //
      if (read != 0)
        in |= mask;
      //
      mask >>= 1;
    }

    write_line (m_ss, high);
    std::this_thread::sleep_for (delay);
  }

  void spi_bitbang_t::write_line (gpiod_line *line, int value)
  {
    if (gpiod_line_set_value (line, value) != 0) {
      log_t::buffer_t buf;
      buf << "bitbang: Failed to write a line";
      log_t::error (buf);
    }
  }

  int spi_bitbang_t::read_line (gpiod_line *line)
  {
    int result = gpiod_line_get_value (line);
    if ((result != 0) && (result != 1)) {
      log_t::buffer_t buf;
      buf << "bitbang: Failed to read a line";
      log_t::error (buf);

      return 0;
    }

    return result;
  }

} // namespace led_d
