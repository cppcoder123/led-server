//
//
//
#include "unix/log.hpp"

#include "clock.hpp"
#include "content.hpp"

namespace led_info_d
{

  content_t::content_t (asio::io_context &context, write_t write)
    : m_current (0),
      m_clock (context),
      m_write (write)
  {
  }

  void content_t::init ()
  {
    m_clock.init (m_info);
  }

  void content_t::push ()
  {
    if (m_info.empty () == true) {
      log_t::error ("info vector is empty");
      return;
    }

    bool status = false;
    std::size_t count = 0;
    do {
      std::string msg;
      if ((status = m_info[m_current] (msg)) == true) {
        request_t request;
        request.action = request_t::insert;
        request.info = msg;
        m_write (request);
      }
      if (++m_current >= m_info.size ())
        m_current = 0;
    } while ((status == false) && (++count < m_info.size ()));
  }
  
} // namespace led_info_d
