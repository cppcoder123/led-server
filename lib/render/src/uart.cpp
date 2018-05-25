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

#include <sstream>
#include <stdexcept>

#include "device-id.h"

#include "uart.hpp"

namespace render
{
  uart_t::uart_t (const std::string &linux_device)
    : m_linux_device (linux_device)
  {
    device_open ();
    // fixme
    // send_receive_check ({MSG_ID_SLAVE_HANDSHAKE});
    // send_receive_check ({MSG_ID_SLAVE_SHIFT_DELAY,
    //       MSG_ID_SHIFT_DELAY, 30, MSG_ID_SHIFT_PAUSE, 150});
    // send_receive_check ({MSG_ID_SLAVE_BRIGHTNESS, MSG_ID_BRIGHTNESS_MAX});
    // send_receive_check ({MSG_ID_SLAVE_INIT});
  }

  uart_t::~uart_t ()
  {
    device_close ();
  }

  bool uart_t::render (const core::matrix_t &info)
  {
    // fixme
    return true;
  }

  bool uart_t::brightness (int level)
  {
    // fixme
    return true;
  }

  void uart_t::device_open ()
  {
    m_descriptor = open (m_linux_device.c_str (), O_RDWR | O_NOCTTY | O_SYNC);
    if (m_descriptor < 0) {
      std::ostringstream stream;
      stream << "Failed to open linux device: ";
      stream << m_linux_device;
      throw std::runtime_error (stream.str ());
    }

    //baudrate 115200, 8 bits, no parity, 1 stop bit
    configure_attributes (B115200);

    //set_min_count (0)
  }

  void uart_t::device_close ()
  {
    close (m_descriptor);
    m_descriptor = -1;
  }
  
  void uart_t::send_receive_check (const msg_t &msg)
  {
    // fixme
  }

  void uart_t::configure_attributes (int speed)
  {
    struct termios tty;

    if (tcgetattr(m_descriptor, &tty) < 0)
      throw std::runtime_error ("Failed to get fd attributes");

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

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
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(m_descriptor, TCSANOW, &tty) != 0) {
      std::stringstream stream ("Failed to set fd attributes, error - ");
      stream << strerror (errno);
      throw std::runtime_error (stream.str ());
    }
  }

  void uart_t::set_min_count (int min_count)
  {
    struct termios tty;

    if (tcgetattr(m_descriptor, &tty) < 0) {
      std::stringstream stream ("Failed to get attributes to set min count, error - ");
      stream << strerror (errno);
      throw std::runtime_error (stream.str ());
    }

    tty.c_cc[VMIN] = min_count ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(m_descriptor, TCSANOW, &tty) < 0) {
      std::stringstream stream ("Failed to set attributes for min count, error - ");
      stream << strerror (errno);
      throw std::runtime_error (stream.str ());
    }
  }

} // namespace render
