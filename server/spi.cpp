/*
 *
 */

#include <fcntl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <thread>

#include "mcu/constant.h"

#include "mcu-decode.hpp"
#include "mcu-encode.hpp"
#include "log-wrapper.hpp"
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

  } // namespace anonymous

  spi_t::spi_t (const std::string &path,
                mcu_queue_t &to_queue, mcu_queue_t &from_queue)
    : m_path (path),
      m_device (0),
      m_go (true),
      m_to_queue (to_queue),
      m_from_queue (from_queue)
  {
    // gpio is first, we need to enable level shifter
    m_gpio.start ();

    // open unix device
    device_start ();
  }

  spi_t::~spi_t ()
  {
    device_stop ();

    m_gpio.stop ();
  }

  void spi_t::start ()
  {

    while (m_go == true) {
      if (m_block.is_engaged () == true) {
        std::this_thread::sleep_for (block_delay);
        continue;
      }
      auto msg = m_to_queue.pop ();
      if (msg)
        write_msg (*msg);
      else if (m_gpio.is_irq_raised () == true)
        // we are interested in gpio-irq only if 'to_queue' is empty
        write_msg (mcu::encode::join (SERIAL_ID_TO_IGNORE, MSG_ID_QUERY));
      else
        std::this_thread::sleep_for (empty_delay);
    }
  }

  void spi_t::stop ()
  {
    m_go = false;

    // fixme: smth else ???
  }

  void spi_t::write_msg (const mcu_msg_t &msg_src)
  {
    char_t serial_id = mcu::decode::get_serial (msg_src);

    //
    // eye-catch | size | serial | msg-id | xxx
    //
    mcu_msg_t msg = msg_src;
    mcu::encode::wrap (msg);

    unsigned i = 0;
    for (const auto &byte : msg)
      write_buffer[i++] = byte;

    if (serial_id != SERIAL_ID_TO_IGNORE)
      m_block.engage (serial_id);

    spi_write ();

    for (unsigned j = 0; j < msg.size (); ++j)
      if (m_parse.push (read_buffer[j], msg) == true) {
        char_t serial = 0;
        char_t msg_id = MSG_ID_EMPTY;
        if (mcu::decode::split (msg, serial, msg_id) == true) {
          if (msg_id == MSG_ID_STATUS)
            m_block.relax (serial);
          else
            m_from_queue.push (msg);
        } else {
          log_t::buffer_t buf;
          buf << "spi: Failed to decode mcu message";
          log_t::error (buf);
        }
      }
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

  void spi_t::device_start ()
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

  void spi_t::device_stop ()
  {
    if (m_device > 0) {
      close (m_device);
      m_device = 0;
    }
  }

} // namespace led_d
