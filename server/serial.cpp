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
#include "final-action.hpp"

#include "log-wrapper.hpp"
#include "serial.hpp"


namespace led_d
{
  namespace {
    static constexpr speed_t port_speed = 500000;
    
    void configure_tty (int descriptor)
    {
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
      tty.c_cc[VTIME] = 1;        // wait 0.1 seconds
      
      if (tcsetattr(descriptor, TCSANOW, &tty) != 0) {
        log_t::buffer_t buf;
        buf << "Failed to set fd attributes, error - "
            << strerror (errno);
        throw std::runtime_error (buf.str ());
      }
    }
  } // namespace anonymous

  serial_t::serial_t (asio::io_service &service, const std::string &device)
    : m_asio_read (std::bind (&serial_t::asio_read, this,
                              std::placeholders::_1, std::placeholders::_2)),
      m_port (service, device),
      m_state (state_first)
  {
    configure_tty (m_port.native_handle ());
    
    m_port.async_read_some
      (asio::buffer (m_read_buffer, read_buffer_size), m_asio_read);
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
        || (src.size () > ID_MAX_SUB_MATRIX_SIZE)
        || (m_flag.test (bit_pending_write) == true))
      return false;

    private_write (src);

    return true;
  }

  void serial_t::private_write (const msg_t &src)
  {
    const char_t msg_size
      = static_cast<char_t>(src.size () - header_right_size);

    msg_t msg = codec_t::encode (ID_EYE_CATCH, msg_size, std::cref (src));

    if (msg.size () > write_buffer_size) {
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

    m_flag.set (bit_pending_write);

    m_port.async_write_some
      (asio::buffer (m_write_buffer, index),
       std::bind (&serial_t::asio_write, this,
                  std::placeholders::_1, std::placeholders::_2, index));
  }

  void serial_t::asio_read (const asio::error_code &code,
                            std::size_t bytes_stransferred)
  {
    if (m_flag.test (bit_pending_read)) {
      log_t::buffer_t buf;
      buf << "serial: Read while doing other read";
      log_t::error (buf);
      return;
    }

    msg_t msg;
    {
      m_flag.set (bit_pending_read);
      auto action = core::make_final_action
        ([this] () {m_flag.reset (bit_pending_read);});

      if (decode_header (msg) == false)
        return;
    }

    if (ready () == false) {
      decode_initial (msg);
      return;
    }
    
    if (m_read == 0) {
      log_t::buffer_t buf;
      buf << "serial: Ready to call external read, but it is empty";
      log_t::error (buf);
      return;
    }

    m_read (msg);

    m_port.async_read_some
      (asio::buffer (m_read_buffer, read_buffer_size), m_asio_read);
  }

  void serial_t::asio_write (const asio::error_code &code,
                             std::size_t bytes_stransferred,
                             std::size_t bytes_expected)
  {
    m_flag.reset (bit_pending_write);

    // fixme

  }
  
} // namespace led_d
