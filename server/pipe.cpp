//
//
//
#include <thread>

#include "device-codec.hpp"
#include "device-id.h"

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
          buf << "Bad status \"" << (int) info << "\" arrived for serial id \""
              << (int) serial_id << "\"";
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
  
  pipe_t::pipe_t (serial_t &serial)
    : m_device (serial),
      m_serial_id (0)
  {
    m_device.bind (std::bind (&pipe_t::decode, this, std::placeholders::_1),
                   std::bind (&pipe_t::write, this));
  }

  pipe_t::~pipe_t ()
  {
  }

  bool pipe_t::render (const core::matrix_t &matrix)
  {
    // if (matrix.size () > ID_MAX_MATRIX_SIZE)
    //   return false;

    bool queue_was_empty = m_write_queue.empty ();

    //header and submatrix-type
    constexpr std::size_t data_size = ID_MAX_SUB_MATRIX_SIZE - ID_HEADER_SIZE - 1;

    // 1. convert
    msg_t matrix_data;
    for (std::size_t i = 0; i < matrix.size (); ++i)
      matrix_data.push_back (get_char (matrix.get_column (i)));

    // 2. split into submatrices
    bool first = true;
    while (matrix_data.empty () == false) {
      auto upto = matrix_data.begin ();
      bool last = (matrix_data.size () <= data_size) ? true : false;
      std::advance (upto, last ? matrix_data.size () : data_size);
      char_t sub_type = (first ? ID_SUB_MATRIX_TYPE_FIRST : 0);
      sub_type |= (last ? ID_SUB_MATRIX_TYPE_LAST : 0);
      sub_type = (sub_type == 0) ? ID_SUB_MATRIX_TYPE_MIDDLE : sub_type;
      msg_t submatrix_data;
      submatrix_data.splice (submatrix_data.begin (), matrix_data,
                             matrix_data.begin (), upto);
      msg_t sub_msg = codec_t::encode
        (get_serial_id (), ID_SUB_MATRIX, sub_type, std::cref (submatrix_data));

      m_write_queue.push (sub_msg);

      first = false;
    }

    if (queue_was_empty == true)
      write ();                 // we should kick writing somehow

    return true;
  }

  void pipe_t::write ()
  {
    if ((m_block.can_go () == false)
        || (m_device.ready () == false))
      return;

    auto opt_msg = m_write_queue.pop ();
    if (opt_msg.has_value () == false)
      return;

    // write changes argument => remember serial_id
    char_t serial_id = opt_msg->front ();
    if (m_device.write (*opt_msg) == true)
      m_block.tighten (serial_id);
    else {
      log_t::buffer_t buf;
      buf << "pipe: Failed to write message \""
          << (int) serial_id << "\" to serial";
      log_t::error (buf);
    }
  }
  
  void pipe_t::decode (msg_t &msg)
  {
    char_t msg_id = 0;
    char_t serial_id = 0;

    if (codec_t::decode_modify
        (msg, std::ref (serial_id), std::ref (msg_id)) == false) {
      log_t::error ("pipe: Failed to decode message header");
      return;
    }

    // header is OK => try to relax blocker
    m_block.relax (serial_id);
    // blocker is relaxed => try to write
    write ();

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
          return;
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
        return;
      }
      break;
    }
  }

  char_t pipe_t::get_serial_id ()
  {
    if (++m_serial_id != ID_DEVICE_SERIAL)
      return m_serial_id;

    return ++m_serial_id;
  }
  
} // namespace led_d
