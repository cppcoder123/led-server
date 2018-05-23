//
//
//

#include "uart.hpp"

#include "pipe.hpp"

namespace led_d
{
  pipe_t::pipe_t (const std::string &device_name)
    : m_queue (),
      m_device (std::make_unique<render::uart_t>(device_name))
  {
  }

  pipe_t::~pipe_t ()
  {
  }

  bool pipe_t::render (core::matrix_t &&matrix)
  {
    m_queue.push (std::move (matrix));
    return true;
  }

} // namespace led_d
