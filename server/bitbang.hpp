//
// Spi bit bang
//
#ifndef BITBANG_HPP
#define BITBANG_HPP

#include <gpiod.h>

#include "type-def.hpp"

namespace led_d
{

  class bitbang_t
  {

  public:
    bitbang_t ();
    ~bitbang_t () {stop ();}

    void start (gpiod_chip *chip);
    void stop ();

    void transfer (const mcu_msg_t &send, mcu_msg_t &receive);

  private:

    // clear spi channel as last step in 'start'
    void drain ();

    void transfer_char (char_t out_char, char_t &in_char);

    void write_line (gpiod_line *line, int value);
    int read_line (gpiod_line *line);

    static constexpr int high = 1;
    static constexpr int low = 0;

    static constexpr unsigned int m_ss_offset = 6;
    static constexpr unsigned int m_mosi_offset = 13;
    static constexpr unsigned int m_clk_offset = 12;
    static constexpr unsigned int m_miso_offset = 19;

    gpiod_line *m_ss;
    gpiod_line *m_mosi;
    gpiod_line *m_clk;
    gpiod_line *m_miso;
  };

} // namespace led_d

#endif
