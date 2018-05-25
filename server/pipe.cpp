//
//
//
#include "device-codec.hpp"
#include "device-id.h"


#include "uart.hpp"

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
      m_serial_id (0)
  {
  }

  pipe_t::~pipe_t ()
  {
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
      (ID_MATRIX, m_serial_id, codec_t::to_char (ID_MATRIX_START),
       std::cref (matrix_submsg), codec_t::to_char (ID_MATRIX_FINISH));

    m_write_queue.push (msg);
    return true;
  }

} // namespace led_d
