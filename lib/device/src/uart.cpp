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
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "device-id.h"
#include "device-codec.hpp"

#include "uart.hpp"

namespace device
{
  uart_t::uart_t (const std::string &linux_device)
    : m_linux_device (linux_device)
  {
    device_open ();

    read_status (ID_STATUS_HELLO);
    
    codec_t::char_t serial_id = 0;

    codec_t::msg_t msg = codec_t::encode (ID_HANDSHAKE, ++serial_id);
    write_status (msg);
    read_status ();

    // fixme: where we shoud keep shift delays?
    msg = codec_t::encode (ID_PIXEL_DELAY, ++serial_id, 5);
    write_status (msg);
    read_status ();

    msg = codec_t::encode (ID_PHRASE_DELAY, ++serial_id, 15);
    write_status (msg);
    read_status ();

    msg = codec_t::encode (ID_STABLE_DELAY, ++serial_id, 50);
    write_status (msg);
    read_status ();

    msg = codec_t::encode (ID_BRIGHTNESS, ++serial_id, ID_BRIGHTNESS_MAX);
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
    // msg size should not be less than 2,
    // msg-id (1 byte) and serial-id (1 byte),
    // so exclude this header info from size to simplify decode

    if ((src.size () < header_right_size)
        || (src.size () > ID_MAX_SUB_MATRIX_SIZE))
      return false;

    const codec_t::char_t msg_size
      = static_cast<codec_t::char_t>(src.size () - header_right_size);

    codec_t::msg_t msg
      = codec_t::encode (ID_EYE_CATCH, msg_size, std::cref (src));

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
    do {
      // try to drain buffer first
      if ((m_message_buffer.empty () == false)
          && (read_decode (msg) == true))
        return true;
      // failed to decode, try to read something
      if ((read_tty () == true)
          && (read_decode (msg) == true))
        return true;
    } while (block == true);

    // block == false
    return m_error.empty ();
  }

  bool uart_t::read_decode (codec_t::msg_t &msg)
  {
    msg.clear ();
    if (m_message_buffer.size () < header_left_size)
      return false;

    codec_t::char_t eye_catch = 0, msg_size = 0;
    if (codec_t::decode (m_message_buffer,
                         std::ref (eye_catch), std::ref (msg_size)) == false)
      // no error just need to wait
      return false;

    if (eye_catch != ID_EYE_CATCH) {
      m_message_buffer.pop_front ();
      m_error += "Expecting eye catch found something different ";
      return false;
    }

    if (msg_size < header_right_size) {
      m_error += "Message size is too small";
      return false;
    }

    std::size_t target_size = header_right_size + msg_size;
    if (m_message_buffer.size () >= header_left_size + target_size) {
      m_message_buffer.pop_front (); // eye catch
      m_message_buffer.pop_front (); // msg size
      while (msg.size () < target_size) {
        auto iter_last = m_message_buffer.begin ();
        std::advance (iter_last, target_size + 1); // range [first, last)
        msg.splice (msg.begin (), m_message_buffer,
                    m_message_buffer.begin (), iter_last);
      }
    }
    
    return true;
  }

  bool uart_t::read_tty ()
  {
    codec_t::char_t buffer[io_max_size];
    int read_length = ::read (m_descriptor, buffer, io_max_size - 1);

    if (read_length > 0) {
      for (int i = 0; i < read_length; ++i)
        m_message_buffer.push_back (buffer[i]);
    } else if (read_length < 0) {
      m_error += "Error during ::read call \"";
      m_error += strerror (errno);
      m_error += "\"";
      return false;
    }

    // we have not zero read timeout, so we should wait there if needed
    return true;
  }

  std::string uart_t::get_error ()
  {
    std::string tmp = m_error;
    m_error.clear ();

    return tmp;
  }
  
  void uart_t::read_status (codec_t::char_t status)
  {
    codec_t::msg_t msg;
    if (read (msg, true) == false) {
      std::ostringstream stream;
      stream << "uart: Failed to read message: \"" << get_error () << "\"";
      throw std::runtime_error (stream.str ());
    }

    codec_t::char_t msg_id = 0;
    codec_t::char_t serial_id = 0;
    if (codec_t::decode_modify
        (msg, std::ref (serial_id), std::ref (msg_id)) == false)
      throw std::runtime_error ("uart: Failed to decode status message");

    if (msg_id != ID_STATUS) {
      std::ostringstream stream;
      stream << "uart: Waiting for status, but \"" << msg_id << "\" is arrived";
      throw std::runtime_error (stream.str ());
    }

    codec_t::char_t msg_status = 0;
    if (codec_t::decode_modify (msg, std::ref (msg_status)) == false)
      throw std::runtime_error ("uart: Failed to decode status value");

    if (msg_status != status) {
      std::ostringstream stream;
      stream << "uart: Arrived status value: \"" << msg_status
             << "\" doesn't match expected one: \"" << status << "\"";
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
    tty.c_cc[VMIN] = 0;         // Note: Don't wait for a byte(s) arrival
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


  
} // namespace device
