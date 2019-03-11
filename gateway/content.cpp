//
//
//

#include "clock.hpp"
#include "content.hpp"

namespace led_info_d
{

  content_t::content_t (asio::io_context &context, write_t write)
    : m_clock (context),
      m_write (write)
  {
  }

  void content_t::init ()
  {
    m_clock.init (m_info);
  }
  
} // namespace led_info_d
