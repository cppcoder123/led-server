//
//
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <algorithm>

#include "mcu/constant.h"
#include "unix/final-action.hpp"
#include "unix/log.hpp"

#include "mcu-msg.hpp"
#include "spi-dev.hpp"

namespace
{
  constexpr const char* path = "/dev/spidev0.0";
  constexpr uint8_t mode = SPI_MODE_0;
  constexpr uint8_t word_size = 8;      // 8 bits per word
  constexpr uint32_t speed = 1000;       // 1kHz ?
  //constexpr uint32_t speed = 20;       // 1kHz ?
  //constexpr uint16_t delay = 1000;      // 1 millisecond ?
  constexpr uint16_t delay = 65535;      // 65 ms?
}

namespace led_d
{

  spi_dev_t::spi_dev_t ()
    : m_device (0)
  {
  }

  spi_dev_t::~spi_dev_t ()
  {
  }

  void spi_dev_t::start ()
  {
    m_device = open (path, O_RDWR);
    if (m_device < 0)
      throw std::runtime_error ("Failed to open spi device");

    uint8_t ioc_mode = mode;
    if (ioctl (m_device, SPI_IOC_WR_MODE, &ioc_mode) < 0)
      throw std::runtime_error ("Failed to set spi write mode");
    if (ioctl (m_device, SPI_IOC_RD_MODE, &ioc_mode) < 0)
      throw std::runtime_error ("Failed to set spi read mode");

    uint8_t ioc_bits = word_size;
    if (ioctl (m_device, SPI_IOC_WR_BITS_PER_WORD, &ioc_bits) < 0)
      throw std::runtime_error ("Failed to set spi write bits");
    if (ioctl (m_device, SPI_IOC_RD_BITS_PER_WORD, &ioc_bits) < 0)
      throw std::runtime_error ("Failed to set spi read bits");

    uint32_t ioc_speed = speed;
    if (ioctl (m_device, SPI_IOC_WR_MAX_SPEED_HZ, &ioc_speed) < 0)
      throw std::runtime_error ("Failed to set spi write speed");
    if (ioctl (m_device, SPI_IOC_RD_MAX_SPEED_HZ, &ioc_speed) < 0)
      throw std::runtime_error ("Failed to set spi read speed");

    drain ();
  }

  void spi_dev_t::stop ()
  {
    if (m_device > 0)
      close (m_device);
    m_device = 0;
  }

  void spi_dev_t::transfer (const mcu_msg_t &out, mcu_msg_t &in)
  {
    static uint8_t *write_buf = nullptr;
    static uint8_t *read_buf = nullptr;
    static std::size_t buf_size = 0;

    // the only purpose of this object is to free memory
    // allocated for the r/w buffers at the end of app execution
    static unix::final_action_t<std::function<void ()>>
      remove_buf ([&] () {delete [] write_buf; delete [] read_buf;});

    auto realloc = [] (uint8_t *&space, std::size_t new_size) {
      delete [] space;
      space = new uint8_t [new_size];
    };

    if (out.size () > buf_size) {
      // it is ok to call delete with null pointer
      realloc (write_buf, out.size ());
      realloc (read_buf, out.size ());
    }

    std::size_t i = 0;
    for (auto info : out)
      write_buf[i++] = info;

    struct spi_ioc_transfer buf;
    memset (&buf, 0, sizeof (buf));
    
    buf.tx_buf =  (unsigned long) write_buf;
    buf.rx_buf =  (unsigned long) read_buf;
    buf.len = out.size ();
    buf.delay_usecs = delay;
    buf.speed_hz = 0;
    buf.bits_per_word = 0;

    errno = 0;
    if (ioctl (m_device, SPI_IOC_MESSAGE(1), &buf) < 0) {
      log_t::buffer_t msg;
      msg << "spi: Failed to send spi message to mcu: " << errno;
      log_t::error (msg);
      return;
    }

    for (i = 0; i < out.size (); ++i)
      in.push_back (read_buf[i]);
  }

  void spi_dev_t::drain ()
  {
    static const std::size_t max_attempt = 6;
    static const std::size_t msg_size = 50;

    for (std::size_t i = 0; i < max_attempt; ++i) {
      mcu_msg_t to_mcu (msg_size, 233);
      // --------------------------------------------------
      // to_mcu.clear ();
      // std::generate_n (to_mcu.begin (), msg_size, [](){return 0;});
      // log_t::buffer_t buf;
      // buf << "spi: to-mcu msg size : " << to_mcu.size ();
      // log_t::info (buf);
      // buf.clear ();
      // --------------------------------------------------
      mcu_msg_t from_mcu;
      // from_mcu.clear ();
      transfer (to_mcu, from_mcu);
      // --------------------------------------------------
      log_t::buffer_t buf;
      buf << "spi: from-mcu msg size : " << from_mcu.size ();
      log_t::info (buf);
      // --------------------------------------------------
      if (std::count (from_mcu.begin (),
                      from_mcu.end (), SPI_WRITE_UNDERFLOW) == msg_size)
        return;
      std::for_each (from_mcu.begin (), from_mcu.end (), [](uint8_t val){
          if (val != SPI_WRITE_UNDERFLOW) {
            log_t::buffer_t buf;
            buf << "AAA: spi-dev: from-mcu not normal : " << (unsigned) val;
            log_t::info (buf);
          }
        });
    }

    throw std::runtime_error ("spi-dev: Failed to drain Spi channel");
  }

} // namespace led_d
