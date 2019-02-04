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
    spi_t ();                   // fixme: delete this constructor
    spi_t (msg_queue_t &from_queue);
    spi_t (const spi_t&) = delete;
    ~spi_t ();

    void start (/*pass arg here*/);
    void stop ();

  private:

    //using msg_t = core::device::codec_t::msg_t;
  
    void write_msg (const msg_t &msg);

    msg_t& query_msg ();

    void spi_write ();

    void device_init ();

    const std::string m_path;   // to device
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
