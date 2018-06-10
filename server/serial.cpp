//
//
//
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <algorithm>
#include <vector>

#include "device-codec.hpp"
#include "device-id.h"

#include "log-wrapper.hpp"
#include "serial.hpp"

namespace led_d
{
  serial_t::serial_t (asio::io_service &service, const std::string &device)
    : m_ready (false),
      m_asio_read (std::bind (&serial_t::asio_read, this,
                              std::placeholders::_1, std::placeholders::_2)),
      m_asio_write (std::bind (&serial_t::asio_write, this,
                               std::placeholders::_1, std::placeholders::_2)),
      m_port (service, device)
  {
    //fixme:: //use descriptor m_port.native_handle ();
    configure_tty ();
    // asio::serial_port_base::baud_rate baud (500000);
    // m_port.set_option (baud);

    // asio::serial_port_base::character_size char_size (8);
    // m_port.set_option (char_size);

    // asio::serial_port_base::flow_control
    //   flow (asio::serial_port_base::flow_control::none);
    // m_port.set_option (flow);

    // asio::serial_port_base::parity
    //   parity (asio::serial_port_base::parity::none);
    // m_port.set_option (parity);

    // asio::serial_port_base::stop_bits stop
    //   (asio::serial_port_base::stop_bits::one);
    // m_port.set_option (stop);
    
    m_port.async_read_some
      (asio::buffer (m_read_buffer, buffer_size), m_asio_read);
  }

  void serial_t::bind (read_t read_cb, write_t write_cb)
  {
    m_read = read_cb;
    m_write = write_cb;
  }

  bool serial_t::write (const msg_t &src)
  {
    if ((ready () == false)
        || (src.size () < header_right_size)
        || (src.size () > ID_MAX_SUB_MATRIX_SIZE))
      return false;

    private_write (src);

    return true;
  }

  void serial_t::configure_tty ()
  {
    int descriptor = m_port.native_handle ();
    
    struct termios tty;
    
    if (tcgetattr(descriptor, &tty) < 0)
      throw std::runtime_error ("Failed to get fd attributes");

    cfsetospeed(&tty, port_speed);
    cfsetispeed(&tty, port_speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 0;         // Note: Don't wait for a byte(s) arrival
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(descriptor, TCSANOW, &tty) != 0) {
      std::stringstream stream ("Failed to set fd attributes, error - ");
      stream << strerror (errno);
      throw std::runtime_error (stream.str ());
    }
    
  }
  
  void serial_t::private_write (const msg_t &src)
  {
    const char_t msg_size
      = static_cast<char_t>(src.size () - header_right_size);

    msg_t msg = codec_t::encode (ID_EYE_CATCH, msg_size, std::cref (src));

    if (msg.size () > buffer_size) {
      log_t::buffer_t buf;
      buf << "serial: Message is longer than buffer size";
      log_t::error (buf);
      return;
    }

    std::size_t index = 0;
    std::for_each (msg.begin (), msg.end (), [this, &index] (char_t info)
                   {
                     m_write_buffer[index++] = info;
                   });
    
    m_port.async_write_some
      (asio::buffer (m_write_buffer, index), m_asio_write);
  }
  
} // namespace led_d
