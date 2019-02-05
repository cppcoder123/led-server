/**
 *
 */
#ifndef SPI_HPP
#define SPI_HPP

#include <string>

//#include "device-codec.hpp"

#include "block.hpp"
#include "spi-gpio.hpp"
#include "spi-parse.hpp"
#include "type-def.hpp"

namespace led_d
{

  class spi_t
  {

  public:
    spi_t (msg_queue_t &from_queue);
    spi_t (const spi_t&) = delete;
    ~spi_t () {}

    void start (const std::string &path);
    void stop ();

  private:

    void write_msg (const msg_t &msg);

    void spi_write ();

    void device_start ();
    void device_stop ();

    std::string m_path;   // to device
    int m_device;               // file descriptor

    bool m_go;

    msg_queue_t m_to_queue;     // to spi
    msg_queue_t &m_from_queue;  // from spi

    spi_gpio_t m_gpio;
    block_t m_block;
    spi_parse_t m_parse;

    static const auto buffer_size = 32;
    static char_t write_buffer[buffer_size];
    static char_t read_buffer[buffer_size];
  };

} // namespace led_d

#endif
