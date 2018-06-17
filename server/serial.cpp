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
#include <iterator>
#include <vector>

#include "device-codec.hpp"
#include "device-id.h"
#include "final-action.hpp"

#include "log-wrapper.hpp"
#include "serial.hpp"


namespace led_d
{
  namespace {
    static constexpr speed_t port_speed = B57600;
    
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
    if (ready () == false)
      return false;
    if  (src.size () < header_right_size)
      return false;
    if  (src.size () > ID_MAX_SUB_MATRIX_SIZE)
      return false;
    if (m_flag.test (bit_pending_write) == true)
      return false;

    bool status = private_write (src);
    if (status == false)
      // Note: possibility of infinite recursion is here
      // though we need to continue writing somehow
      m_write ();

    return status;
  }

  bool serial_t::private_write (const msg_t &src)
  {
    const char_t msg_size
      = static_cast<char_t>(src.size () - header_right_size);

    msg_t msg = codec_t::encode (ID_EYE_CATCH, msg_size, std::cref (src));

    if (msg.size () > write_buffer_size) {
      log_t::buffer_t buf;
      buf << "serial: Message is longer than buffer size";
      log_t::error (buf);
      return false;
    }

    std::size_t index = 0;
    std::for_each (msg.begin (), msg.end (), [this, &index] (char_t info)
                   {
                     m_write_buffer[index++] = info;
                   });

    m_flag.set (bit_pending_write);

    debug_print ("writing", m_write_buffer, index);
    
    m_port.async_write_some
      (asio::buffer (m_write_buffer, index),
       std::bind (&serial_t::asio_write, this,
                  std::placeholders::_1, std::placeholders::_2, index));
    return true;
  }

  void serial_t::asio_read (const asio::error_code &code,
                            std::size_t bytes_transferred)
  {
    if (m_flag.test (bit_pending_read)) {
      log_t::buffer_t buf;
      buf << "serial: Read while doing other read";
      log_t::error (buf);
      return;
    }

    // setup read callback right before return from the function
    auto read_guard = core::make_final_action
      ([this] ()
       {
         m_port.async_read_some
         (asio::buffer (m_read_buffer, read_buffer_size), m_asio_read);
       });
    
    if (code) {
      log_t::buffer_t buf;
      buf << "serial: Not zero error code during read";
      log_t::error (buf);
      return;
    }

    // more than one message can arrive at once
    msg_list_t msg_list;
    {
      m_flag.set (bit_pending_read);
      auto action = core::make_final_action
        ([this] () {m_flag.reset (bit_pending_read);});

      if (decode_split (msg_list, bytes_transferred) == false)
        return;
    }

    std::for_each
      (msg_list.begin (), msg_list.end (),
       [this] (msg_t &msg)
       {
         if (ready () == true)
           m_read (msg);
         else
           decode_initial (msg);
       });
  }

  void serial_t::asio_write (const asio::error_code &code,
                             std::size_t bytes_transferred,
                             std::size_t bytes_expected)
  {
    m_flag.reset (bit_pending_write);

    auto next
      = core::make_final_action
      ([this] () { if (ready () == true) m_write ();});

    if (code) {
      log_t::buffer_t buf;
      buf << "serial: Not zero error code during write";
      log_t::error (buf);
      return;
    }
    if (bytes_transferred != bytes_expected) {
      log_t::buffer_t buf;
      buf << "serial: Expected to transfer \"" << bytes_expected
          << "\" bytes, but actually transferred \""
          << bytes_transferred << "\".";
      log_t::error (buf);
      return;
    }
  }

  bool serial_t::decode_split (msg_list_t &msg_list, std::size_t bytes_transferred)
  {
    debug_print ("reading", m_read_buffer, bytes_transferred);

    for (std::size_t i = 0; i < bytes_transferred; ++i)
          m_msg_buffer.push_back (m_read_buffer[i]);

    // Note: actually we can read from serial again here

    while (m_msg_buffer.size () >= header_left_size + header_right_size) {
      char_t symbol = 0, size = 0;
      if (codec_t::decode
          (m_msg_buffer, std::ref (symbol), std::ref (size)) == false) {
        log_t::buffer_t buf;
        buf << "serial: Failed to decode message header";
        log_t::error (buf);
        return !msg_list.empty ();
      }
      if (symbol != ID_EYE_CATCH) {
        log_t::buffer_t buf;
        buf << "serial: Expecting eye-catch, but got \"" << (int) symbol
            << "\" instead, skipping";
        log_t::error (buf);
        m_msg_buffer.pop_front ();
        continue;
      }

      std::size_t total_size = size + header_left_size + header_right_size;
      if (m_msg_buffer.size () < total_size)
        // need to wait, probably bytes will arrive later
        return !msg_list.empty ();

      m_msg_buffer.pop_front (); // eye-catcher
      m_msg_buffer.pop_front (); // size
      total_size -= 2;

      auto iter_last = m_msg_buffer.begin ();
      std::advance (iter_last, total_size);
      msg_t msg;
      msg.splice (msg.begin (), m_msg_buffer,
                  m_msg_buffer.begin (), iter_last);
      msg_list.push_back (msg);
    }

    return !msg_list.empty ();
  }

  void serial_t::decode_initial (msg_t &msg)
  {
    // we are expecting only status msg here
    char_t serial_id = 0, msg_id = 0, status = 0;
    if (codec_t::decode_modify (msg, std::ref (serial_id), std::ref (msg_id),
                                std::ref (status)) == false) {
      log_t::buffer_t buf;
      buf << "serial: Failed to decode status message in \"decode_initial\"";
      log_t::error (buf);
      return;
    }
    // ignore now
    // if (serial_id != ID_DEVICE_SERIAL) {
    //   log_t::buffer_t buf;
    //   buf << "serial: Expecting serial id \""
    //       << ID_DEVICE_SERIAL << "\", but got \"" << serial_id
    //       << "\" instead, ignoring";
    //   log_t::error (buf);
    // }

    log_t::buffer_t buf;

    switch (msg_id) {
    case ID_HEADER_DECODE_FAILED:
      buf << "serial: Header decode failure message is arrived \""
          << (int) status << "\"";
      break;
    case ID_STATUS:
      buf << "serial: Status message is arrived";
      break;
    case ID_BRIGHTNESS:
      buf << "serial: Brightness message is arrived";
      if (m_state == state_brightness)
        m_state = state_init;
      break;
    case ID_HANDSHAKE:
      buf << "serial: Handshake message is arrived";
      if (m_state == state_handshake)
        m_state = state_pixel_delay;
      break;
    case ID_INIT:
      buf << "serial: Init message is arrived";
      if (m_state == state_init)
        m_state = state_last;
      break;
    case ID_PIXEL_DELAY:
      buf << "serial: Pixel delay message is arrived";
      if (m_state == state_pixel_delay)
        m_state = state_phrase_delay;
      break;
    case ID_PHRASE_DELAY:
      buf << "serial: Phrase delay message is arrived";
      if (m_state == state_phrase_delay)
        m_state = state_stable_delay;
      break;
    case ID_STABLE_DELAY:
      buf << "serial: Stable delay message is arrived";
      if (m_state == state_stable_delay)
        m_state = state_brightness;
      break;
    case ID_UNINIT:             // ???
      buf << "serial: ??? Uninit message is arrived";
      break;
    default:
      buf << "serial: ??? Unknown message \"" << (int) msg_id
          << "\" is arrived in \"handshake\"";
      break;
    }

    log_t::info (buf);
  }

  bool serial_t::handshake ()
  {
    if (m_state >= state_last) {
      m_flag.set (bit_ready);
      if ((m_read == 0) || (m_write == 0)) {
        log_t::buffer_t buf;
        buf << "serial: Ready to call user callbacks but they are empty";
        log_t::error (buf);
      } else {
        // we have completed init, so we can write now
        m_write ();
      }
      return false;
    }

    bool empty = false;
    char_t msg_id = 0, msg_value = 0;

    switch (m_state) {
    case state_handshake:
      msg_id = ID_HANDSHAKE;
      empty = true;
      break;
    case state_pixel_delay:
      msg_id = ID_PIXEL_DELAY;
      msg_value = 6;            // ??? 0.1 sec
      break;
    case state_phrase_delay:
      msg_id = ID_PHRASE_DELAY;
      msg_value = 12;           // ???
      break;
    case state_stable_delay:
      msg_id = ID_STABLE_DELAY;
      msg_value = 200;          // ??? 3 sec
      break;
    case state_brightness:
      msg_id = ID_BRIGHTNESS;
      msg_value = ID_BRIGHTNESS_MAX;
      break;
    case state_init:
      msg_id = ID_INIT;
      empty = true;
      break;
    default:
      {
        log_t::buffer_t buf;
        buf << "serial: Unknown state in \"handshake\"";
        log_t::error (buf);
        return true;
      }
      break;
    }

    static char_t serial_id = 0;
    msg_t msg = (empty == true) ? codec_t::encode (++serial_id, msg_id)
      : codec_t::encode (++serial_id, msg_id, msg_value);

    private_write (msg);

    return true;
  }

  void serial_t::debug_print (const std::string &tag,
                              char_t *data, std::size_t data_size)
  {
    log_t::buffer_t buf;
    buf << " Debug: " << tag << ": ";
    for (std::size_t i = 0; i < data_size; ++i)
      buf << (int) data[i] << " ";

    log_t::info (buf);
  }
  
} // namespace led_d
