/*
 *
 */

#include <fcntl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

#include <thread>

#include "mcu/constant.h"

#include "msg.hpp"
#include "spi.hpp"

namespace led_d
{

  char_t spi_t::write_buffer[buffer_size];
  char_t spi_t::read_buffer[buffer_size];

  namespace {

    auto block_delay = std::chrono::milliseconds (300);
    auto empty_delay = std::chrono::milliseconds (300);

    // fixme: !!!
    auto spi_delay = 5;
    auto spi_mode = 0;
    auto spi_bits = 8;
    auto spi_speed = 1000;

    // const auto buffer_size = 32;
    // char_t write_buffer[buffer_size];
    // char_t read_buffer[buffer_size];
    
  } // namespace anonymous

  void spi_t::start ()
  {
    // gpio is first, we need to enable level shifter
    m_gpio.start ();

    // open unix device
    device_init ();

    while (m_go == true) {
      if (m_block.is_engaged () == true) {
        std::this_thread::sleep_for (block_delay);
        continue;
      }
      auto msg = m_to_queue.pop ();
      if (msg.has_value () == true)
        write_msg (*msg);
      else if (m_gpio.is_irq_raised () == true)
        // we are interested in gpio-irq only if 'to_queue' is empty
        write_msg (query_msg ());
      else
        std::this_thread::sleep_for (empty_delay);
    }
  }

  void spi_t::write_msg (const msg_t &msg_src)
  {
    char_t serial_id = msg_get_serial (msg_src);

    //
    // eye-catch | size | serial | msg-id | xxx
    //
    msg_t msg = msg_src;
    msg.push_front (msg.size ());
    msg.push_front (ID_CATCH_EYE);

    unsigned i = 0;
    for (const auto &byte : msg)
      write_buffer[i++] = byte;

    if (serial_id != SERIAL_ID_TO_IGNORE)
      m_block.engage (serial_id);

    spi_write ();

    for (unsigned j = 0; j < msg.size (); ++j)
      if (m_parse.push (read_buffer[j], msg) == true) {
        if (msg_get_id (msg) == MSG_ID_STATUS)
          m_block.relax (msg_get_serial (msg));
        else
          m_from_queue.push (msg);
      }
  }

  msg_t& spi_t::query_msg ()
  {
    static msg_t msg{SERIAL_ID_TO_IGNORE, MSG_ID_QUERY, MSG_ID_QUERY};

    return msg;
  }

  void spi_t::spi_write ()
  {
    spi_ioc_transfer buf;
    buf.tx_buf =  (unsigned long) write_buffer;
    buf.rx_buf =  (unsigned long) read_buffer;
    buf.len = buffer_size;
    buf.delay_usecs = spi_delay;
    buf.speed_hz = 0;
    buf.bits_per_word = 0;

    if (ioctl (m_device, SPI_IOC_MESSAGE (1), &buf) < 0) {
      // fixme: log complain
    }
  }

  void spi_t::device_init ()
  {
    m_device = open (m_path.c_str (), O_RDWR);
    if (m_device < 0)
      throw std::runtime_error ("Failed to open spi device");
    if (ioctl (m_device, SPI_IOC_WR_MODE, &spi_mode) < 0)
      throw std::runtime_error ("Failed to set spi write mode");
    if (ioctl (m_device, SPI_IOC_RD_MODE, &spi_mode) < 0)
      throw std::runtime_error ("Failed to set spi read mode");
    if (ioctl (m_device, SPI_IOC_WR_BITS_PER_WORD, &spi_bits) < 0)
      throw std::runtime_error ("Failed to set spi write bits");
    if (ioctl (m_device, SPI_IOC_RD_BITS_PER_WORD, &spi_bits) < 0)
      throw std::runtime_error ("Failed to set spi read bits");
    if (ioctl (m_device, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0)
      throw std::runtime_error ("Failed to set spi write speed");
    if (ioctl (m_device, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed) < 0)
      throw std::runtime_error ("Failed to set spi read speed");
  }

} // namespace led_d
