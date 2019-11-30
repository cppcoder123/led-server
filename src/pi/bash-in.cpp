//
//
//
#include "unix/log.hpp"

#include "bash-in.hpp"

namespace led_d
{

  constexpr auto mpd_query_name = "led-mpd.sh";

  bash_in_t::bash_in_t (asio::io_context &io_context, bash_queue_t &queue)
    : m_context (io_context),
      m_queue (queue)
  {
  }

  void bash_in_t::start ()
  {
    log_t::buffer_t buf;
    buf << "bash-in: Starting service...";
    log_t::info (buf);

    auto popen = std::make_shared<popen_t>
      (mpd_query_name, m_context, m_queue);
    m_popen_list.push_back (popen);
  }

  void bash_in_t::stop ()
  {
    log_t::buffer_t buf;
    buf << "bash-in: Stopping the service...";
    log_t::info (buf);

    m_context.stop ();

    log_t::clear (buf);
    buf << "bash-in: Service is stopped";
    log_t::info (buf);

    m_popen_list.clear ();

    // fixme: Do we need to do smth else here?
  }

} // namespace led_d
