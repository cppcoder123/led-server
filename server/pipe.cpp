//
//
//
#include <thread>

#include "device-codec.hpp"
#include "device-id.h"

#include "uart.hpp"

#include "log-wrapper.hpp"
#include "pipe.hpp"

namespace led_d
{
  namespace
  {
    using char_t = core::device::codec_t::char_t;

    char_t get_char (const core::matrix_t::column_t &column)
    {
      char_t res = 0, mask = 1;
      for (std::size_t bit = 0; bit < core::matrix_t::column_size; ++bit) {
        if (column.test (bit) == true)
          res |= mask;
        mask <<= 1;
      }

      return res;
    }

    void decode_1_char_msg (char_t msg_id, char_t serial_id, char_t info)
    {
      log_t::buffer_t buf;
      bool complain = true;

      switch (msg_id) {
      case ID_HEADER_DECODE_FAILED:
        buf << "Device failed to decode header for message with serial id \""
            << serial_id << "\"";
        break;
      case ID_STATUS:
        if (info != ID_STATUS_OK) {
          buf << "Bad status \"" << info << "\" arrived for serial id \""
              << serial_id << "\"";
        } else {
          complain = false;
        }
        break;
      case ID_INVALID_MSG:
        buf << "Device failed to recognize \"" << info
            << "\" as message id, serial id \"" << serial_id << "\"";
        break;
      case ID_BUTTON:
        buf << "Not implemented \"button\" message is arrived, value \""
            << info << "\"";
        break;
      default:
        {
          buf << "Unknown message id \"" << msg_id << "\", serial id \""
              << serial_id << "\"";
          log_t::error (buf);
        }
        break;
      }

      if (complain == true)
        log_t::error (buf);
    }
  } // namespace anonymous
  
  pipe_t::pipe_t (const std::string &device_name)
    : m_device (std::make_unique<device::uart_t>(device_name)),
      m_serial_id (0),
      m_device_go (true),
      m_device_thread (std::thread (&pipe_t::serve_read_write, this))
  {
  }

  pipe_t::~pipe_t ()
  {
    m_device_go = false;
    m_device_thread.join ();
  }

  bool pipe_t::render (const core::matrix_t &matrix)
  {
    if (matrix.size () > ID_MAX_MATRIX_SIZE)
      return false;

    for (std::size_t sub_index = 0;
         sub_index * ID_MAX_SUB_MATRIX_SIZE <= matrix.size (); ++sub_index) {
      msg_t info_msg;
      for (std::size_t i = sub_index * ID_MAX_SUB_MATRIX_SIZE;
           i < (sub_index + 1) * ID_MAX_SUB_MATRIX_SIZE; ++i)
        info_msg.push_back (get_char (matrix.get_column (i)));
      char_t submsg_type = (sub_index == 0) ? ID_SUB_MATRIX_TYPE_FIRST : 0;
      if (((sub_index + 1)* ID_MAX_SUB_MATRIX_SIZE) >= matrix.size ())
        submsg_type |= ID_SUB_MATRIX_TYPE_LAST;
      if ((submsg_type & ID_SUB_MATRIX_TYPE_MASK) == 0)
        // neither first, nor last => middle
        submsg_type |= ID_SUB_MATRIX_TYPE_MIDDLE;
      msg_t sub_msg = codec_t::encode
        (ID_SUB_MATRIX, get_serial_id (), std::cref (info_msg));

      m_write_queue.push (sub_msg);
    }

    return true;
  }

  void pipe_t::serve_read_write ()
  {
    while (m_device_go == true) {
      {
        if (m_block.can_go () == true) {
          auto opt_msg = m_write_queue.pop ();
          if (opt_msg.has_value () == true) {
            if (m_device->write (*opt_msg) == true) {
              // Note: front char_t is serial id
              m_block.tighten (opt_msg->front ());
            } else {
              log_t::error ("pipe: Failed to write message");
            }
          }
        }
      }
      {
        msg_t msg;
        if (m_device->read (msg, false) == true) {
          if (decode (msg) == false) {
            log_t::error ("pipe: Failed to decode message");
          }
        }
      }
    }
  }

  bool pipe_t::decode (msg_t &msg)
  {
    char_t msg_id = 0;
    char_t serial_id = 0;

    if (codec_t::decode_modify
        (msg, std::ref (serial_id), std::ref (msg_id)) == false) {
      log_t::error ("pipe: Failed to decode message header");
      return false;
    }

    // header is OK => try to relax blocker
    m_block.relax (serial_id);

    switch (msg_id) {
    case ID_HEADER_DECODE_FAILED:
    case ID_STATUS:
    case ID_BUTTON:
    case ID_INVALID_MSG:
      {
        char_t info = 0;
        if (codec_t::decode_modify (msg, std::ref (info)) == false) {
          log_t::buffer_t buf;
          buf << "pipe: Failed to decode message body, message id: \""
              << msg_id << "\", serial id: \"" << serial_id << "\"";
          log_t::error (buf);
          return false;
        }
        decode_1_char_msg (msg_id, serial_id, info);
      }
      break;
    default:
      {
        log_t::buffer_t buf;
        buf << "Unknown message is arrived with id \"" << msg_id
            << "\", serial id \"" << serial_id << "\"";
        log_t::error (buf);
        return false;
      }
      break;
    }

    return true;
  }

  char_t pipe_t::get_serial_id ()
  {
    if (++m_serial_id != ID_ARDUINO_SERIAL)
      return m_serial_id;

    return ++m_serial_id;
  }
  
} // namespace led_d
