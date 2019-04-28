/**
 *
 */
#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <stdint.h>

#include <string>

#include "block.hpp"
#include "gpio.hpp"
#include "handle.hpp"
#include "parse.hpp"
#include "spi.hpp"
#include "type-def.hpp"

namespace led_d
{

  class device_t
  {

  public:
    device_t (const std::string &path,
           mcu_queue_t &to_queue, mcu_queue_t &from_queue, bool show_msg);
    device_t (const device_t&) = delete;
    ~device_t ();

    void start ();
    void stop ();

  private:

    void write_msg (const mcu_msg_t &msg);

    // void device_write (uint32_t msg_size);
    // void device_start ();
    // void device_stop ();

    // const std::string m_path;   // to device
    // int m_device;               // file descriptor

    bool m_go;

    mcu_queue_t &m_to_queue;    // to spi
    mcu_queue_t &m_from_queue;  // from spi

    gpio_t m_gpio;
    spi_t m_spi;

    block_t m_block;
    parse_t m_parse;

    bool m_show_msg;
  };

} // namespace led_d

#endif
