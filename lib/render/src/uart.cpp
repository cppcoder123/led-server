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
#include <chrono>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "device-id.h"
#include "device-codec.hpp"

#include "uart.hpp"

namespace render
{
  uart_t::uart_t (const std::string &linux_device)
    : m_linux_device (linux_device)
  {
    device_open ();

    codec_t::short_t serial_id = 0;

    codec_t::msg_t msg = codec_t::encode (ID_HANDSHAKE, ++serial_id);
    write_status (msg);
    read_status ();

    // fixme: where we shoud keep shift delays?
    msg = codec_t::encode (ID_SHIFT_DELAY, ++serial_id,
                           codec_t::to_short (200), codec_t::to_short (700));
    write_status (msg);
    read_status ();

    msg = codec_t::encode (ID_BRIGHTNESS, ++serial_id,
                           codec_t::to_char (ID_BRIGHTNESS_MAX));
    write_status (msg);
    read_status ();

    msg = codec_t::encode (ID_INIT, ++serial_id);
    write_status (msg);
    read_status ();
  }

  uart_t::~uart_t ()
  {
    device_close ();
  }

  // 
  bool uart_t::write (const codec_t::msg_t &src)
  {
    codec_t::msg_t msg = codec_t::encode
      (ID_EYE_CATCH, codec_t::to_short (src.size ()), std::cref (src));

    codec_t::char_t buffer[io_max_size];
    std::size_t io_size = 0;
    std::size_t size = 0;
    
    for (codec_t::char_t symbol : msg) {
      buffer[io_size++] = symbol;
      if ((io_size == io_max_size) || (size + io_size == msg.size ())) {
        if (::write (m_descriptor, buffer, io_size) == false) {
          std::ostringstream stream;
          stream << "uart: Failed to write to linux device: \""
                 << strerror (errno) << "\"";
          m_error = stream.str ();
          return false;
        }
        tcdrain (m_descriptor);
        //
        size += io_size;
        io_size = 0;
      }
    }

    return true;
  }

  // 
  bool uart_t::read (codec_t::msg_t &msg, bool block)
  {
    bool started = false, finished = false;
    std::size_t msg_size = 0;

    m_error.clear ();
    
    do {
      // try to drain message buffer first
      unwrap_fill (msg, started, msg_size);
      if ((started == true)
          && (msg_size != 0)
          && (msg.size () == msg_size))
        return true;

      // buffer was empty or message arrived partly => read more
      codec_t::char_t buffer[io_max_size];
      int read_length = ::read (m_descriptor, buffer, io_max_size - 1);
      if (read_length > 0) {
        for (int i = 0; i < read_length; ++i)
          m_message_buffer.push_back (buffer[i]);
        unwrap_fill (msg, started, msg_size);
      } else if (read_length < 0) {
        m_error += "Error during ::read call \"";
        m_error += strerror (errno);
        m_error += "\"";
        return false;
      } else {
        // no error just need to wait in case of blocking read
        if (block == true)
          // fixme: where we should keep read delay?
          std::this_thread::sleep_for (std::chrono::milliseconds (100));
      }
      finished = ((msg_size != 0) && (msg_size == msg.size ()));
    } while (((block == false) && (started == true) && (finished == false)) // at least 1 symbol is arrived
             || ((block == true) && (finished == false))); // read till message is arrived

    return m_error.empty ();
  }

  void uart_t::unwrap_fill (codec_t::msg_t &msg, bool &started, std::size_t &msg_size)
  {
    std::for_each (m_message_buffer.begin (), m_message_buffer.end (),
      [&msg, &started, &msg_size] (codec_t::char_t info)
      {
        if (started == false) {
          if (info == ID_EYE_CATCH) {
            started = true;
          } else {
            // we are missing message start?
            // m_error += "Skipping symbol: \"" + buffer[index] + "\" ";
          }
        } else {                // started == true
          if (msg_size == 0) {
            // we do not expect long messages, so 255 should be enough
            msg_size = info;
          } else {              // msg_size != 0
            msg.push_back (info);
            if (msg.size () == msg_size)
              // the rest of read info should stay in (m_message_) buffer
              return;
          }
        }
      });
  }

  std::string uart_t::get_error ()
  {
    std::string tmp = m_error;
    m_error.clear ();

    return tmp;
  }
  
  void uart_t::read_status ()
  {
    codec_t::msg_t msg;
    if (read (msg, true) == false) {
      std::ostringstream stream;
      stream << "uart: Failed to read message: \"" << get_error () << "\"";
      throw std::runtime_error (stream.str ());
    }

    codec_t::char_t msg_id = 0;
    codec_t::short_t serial_id = 0;
    if (codec_t::decode_head (msg, msg_id, serial_id) == false)
      throw std::runtime_error ("uart: Failed to decode status message");

    if (msg_id != ID_STATUS) {
      std::ostringstream stream;
      stream << "uart: Waiting for status, but \"" << msg_id << "\" is arrived";
      throw std::runtime_error (stream.str ());
    }

    codec_t::char_t msg_status = 0;
    if (codec_t::decode_body (msg, msg_status) == false)
      throw std::runtime_error ("uart: Failed to decode status value");

    if (msg_status != ID_STATUS_OK) {
      std::ostringstream stream;
      stream << "uart: Wrong status value: \"" << msg_status << "\"";
      throw std::runtime_error (stream.str ());
    }
  }

  void uart_t::write_status (const codec_t::msg_t &msg)
  {
    if (write (msg) == false) {
      std::ostringstream stream;
      stream << "Failed to write \"" << get_error () << "\"";
      throw std::runtime_error (stream.str ());
    }
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
