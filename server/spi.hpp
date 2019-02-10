/**
 *
 */
#ifndef SPI_HPP
#define SPI_HPP

#include <string>

#include "block.hpp"
#include "handle.hpp"
#include "gpio.hpp"
#include "spi-parse.hpp"
#include "type-def.hpp"

namespace led_d
{

  class spi_t
  {

  public:
    spi_t (const std::string &path,
           mcu_queue_t &to_queue, mcu_queue_t &from_queue);
    spi_t (const spi_t&) = delete;
    ~spi_t ();

    void start ();
    void stop ();

  private:

    void write_msg (const mcu_msg_t &msg);

    void spi_write ();

    void device_start ();
    void device_stop ();

    const std::string m_path;   // to device
    int m_device;               // file descriptor

    bool m_go;

    mcu_queue_t &m_to_queue;    // to spi
    mcu_queue_t &m_from_queue;  // from spi

    gpio_t m_gpio;
    block_t m_block;
    spi_parse_t m_parse;

    static const auto buffer_size = 32;
    static char_t write_buffer[buffer_size];
    static char_t read_buffer[buffer_size];
  };

} // namespace led_d

#endif
