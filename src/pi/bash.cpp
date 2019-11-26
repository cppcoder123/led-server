//
//
//
#include "unix/log.hpp"

#include "bash.hpp"

namespace led_d
{

  constexpr auto mpd_query_name = "led-mpd.sh";

  bash_t::bash_t (asio::io_context &io_context, bash_queue_t &queue)
    : m_context (io_context),
      m_queue (queue)
  {
  }

  void bash_t::start ()
  {
    log_t::buffer_t buf;
    buf << "bash: Starting service...";
    log_t::info (buf);

    auto popen = std::make_shared<popen_t>(mpd_query_name, true, m_context);
    m_popen_list.push_back (popen);
    auto &descriptor = popen->descriptor ();
    descriptor.async_wait
      (asio::posix::stream_descriptor::wait_read,
       std::bind (&bash_t::handle_popen, this, popen, std::placeholders::_1));
  }

  void bash_t::stop ()
  {
    log_t::buffer_t buf;
    buf << "bash: Stopping the service...";
    log_t::info (buf);

    m_context.stop ();

    log_t::clear (buf);
    buf << "bash: Service is stopped";
    log_t::info (buf);

    // fixme: Do we need to do smth else here?
  }

  void bash_t::handle_popen (popen_ptr_t popen, const asio::error_code &errc)
  {
    if (errc) {
      log_t::buffer_t buf;
      buf << "bash: Failed to handle popen event";
      log_t::error (buf);
    }

    auto info = popen->read ();
    if (info.empty () == false)
      m_queue.push (info);

    auto &descriptor = popen->descriptor ();
    descriptor.async_wait
      (asio::posix::stream_descriptor::wait_read,
       std::bind (&bash_t::handle_popen, this, popen, std::placeholders::_1));
  }

} // namespace led_d
