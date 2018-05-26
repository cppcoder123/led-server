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
    core::device::codec_t::char_t get_char (const core::matrix_t::column_t &column)
    {
      core::device::codec_t::char_t res = 0, mask = 1;
      for (std::size_t bit = 0; bit < core::matrix_t::column_size; ++bit) {
        if (column.test (bit) == true)
          res |= mask;
        mask <<= 1;
      }

      return res;
    }
  } // namespace anonymous
  
  pipe_t::pipe_t (const std::string &device_name)
    : m_device (std::make_unique<render::uart_t>(device_name)),
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

    ++m_serial_id;

    using core::device::codec_t;

    codec_t::msg_t matrix_submsg;
    for (std::size_t i = 0; i < matrix.size (); ++i)
      matrix_submsg.push_back (get_char (matrix.get_column (i)));
    
    codec_t::msg_t msg = codec_t::encode
      (ID_MATRIX, m_serial_id,
       codec_t::to_short (matrix_submsg.size ()), std::cref (matrix_submsg));

    m_write_queue.push (msg);
    return true;
  }

  void pipe_t::serve_read_write ()
  {
    while (m_device_go == true) {
      {
        auto opt_msg = m_write_queue.pop ();
        if (opt_msg.has_value () == true) {
          if (m_device->write (*opt_msg) == false) {
            log_t::error ("pipe: Failed to write message");
          }
        }
      }
      {
        codec_t::msg_t msg;
        if (m_device->read (msg, false) == true) {
          if (decode (msg) == false) {
            log_t::error ("pipe: Failed to decode message");
          }
        }
      }
    }
  }

  bool pipe_t::decode (codec_t::msg_t &msg)
  {
    codec_t::char_t msg_id = 0;
    codec_t::short_t serial_id = 0;

    if (codec_t::decode_head (msg, msg_id, serial_id) == false) {
      log_t::error ("pipe: Failed to decode message header");
      return false;
    }

    switch (msg_id) {
    case ID_STATUS:
      {
        codec_t::char_t status = 0;
        if (codec_t::decode_body (msg, status) == false) {
          log_t::buffer_t buf;
          buf << "pipe: Failed to decode message body, message id: \""
              << msg_id << "\", serial id: \"" << serial_id << "\"";
          log_t::error (buf);
          return false;
        }
        if (status != ID_STATUS_OK) {
          log_t::buffer_t buf;
          buf << "pipe: Bad status \"" << status << "\", is arrived for message id \""
              << msg_id << "\", serial id \"" << serial_id << "\"";
          log_t::error (buf);
          return false;
        }
      }
      break;
      // fixme: add button handling here
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
} // namespace led_d
