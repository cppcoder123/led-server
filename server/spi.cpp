//
//
//

#include <stdint.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <algorithm>

#include "mcu/constant.h"
#include "unix/final-action.hpp"
#include "unix/log.hpp"

#include "spi.hpp"

namespace
{
  constexpr const char* path = "/dev/spidev1.0";
  constexpr unsigned long mode = SPI_MODE_0;
  constexpr unsigned long word_size = 0;      // 8 bits per word
  constexpr unsigned long speed = 1000;       // 1kHz ?
  constexpr unsigned short delay = 1000;      // 1 millisecond ?
}

namespace led_d
{

  spi_t::spi_t ()
    : m_device (0)
  {
  }

  spi_t::~spi_t ()
  {
  }

  void spi_t::start ()
  {
    m_device = open (path, O_RDWR);
    if (m_device < 0)
      throw std::runtime_error ("Failed to open spi device");

    if (ioctl (m_device, SPI_IOC_WR_MODE, &mode) < 0)
      throw std::runtime_error ("Failed to set spi write mode");
    // if (ioctl (m_device, SPI_IOC_RD_MODE, &mode) < 0)
    //   throw std::runtime_error ("Failed to set spi read mode");
    if (ioctl (m_device, SPI_IOC_WR_BITS_PER_WORD, &word_size) < 0)
      throw std::runtime_error ("Failed to set spi write bits");
    // if (ioctl (m_device, SPI_IOC_RD_BITS_PER_WORD, &word_size) < 0)
    //   throw std::runtime_error ("Failed to set spi read bits");
    if (ioctl (m_device, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
      throw std::runtime_error ("Failed to set spi write speed");
    // if (ioctl (m_device, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
    //   throw std::runtime_error ("Failed to set spi read speed");

    drain ();
  }

  void spi_t::stop ()
  {
    if (m_device > 0)
      close (m_device);
    m_device = 0;
  }

  void spi_t::transfer (const mcu_msg_t &out, mcu_msg_t &in)
  {
    static char *write_buf = nullptr;
    static char *read_buf = nullptr;
    static std::size_t buf_size = 0;

    // the only purpose of this object is to free memory
    // allocated for the r/w buffers at the end of app execution
    static unix::final_action_t<std::function<void ()>>
      remove_me ([&] () {delete [] write_buf; delete [] read_buf;});

    auto realloc = [] (char *space, std::size_t new_size) {
      delete [] space;
      space = new char [new_size];
    };

    if (out.size () > buf_size) {
      // it is ok to call delete with null pointer
      realloc (write_buf, out.size ());
      realloc (read_buf, out.size ());
    }

    spi_ioc_transfer buf;
    buf.tx_buf =  (unsigned long) write_buf;
    buf.rx_buf =  (unsigned long) read_buf;
    buf.len = out.size ();
    buf.delay_usecs = delay;
    buf.speed_hz = 0;
    buf.bits_per_word = 0;

    if (ioctl (m_device, SPI_IOC_MESSAGE (1), &buf) < 0) {
      log_t::buffer_t buf;
      buf << "spi: Failed to send spi message to mcu";
      log_t::error (buf);
    }
  }

  void spi_t::drain ()
  {
    static const std::size_t max_attempt = 20;
    static const std::size_t msg_size = 50;

    mcu_msg_t to_mcu, from_mcu;

    for (std::size_t i = 0; i < max_attempt; ++i) {
      to_mcu.clear ();
      std::fill_n (to_mcu.begin (), msg_size, 0);
      from_mcu.clear ();
      transfer (to_mcu, from_mcu);
      if (std::count (from_mcu.begin (),
                      from_mcu.end (), SPI_WRITE_UNDERFLOW) == msg_size)
        return;
    }

    throw std::runtime_error ("spi: Failed to drain Spi channel");
  }

} // namespace led_d
