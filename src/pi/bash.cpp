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

    auto mpd = std::make_shared<popen_t>(mpd_query_name, true, m_context);
    m_popen_list.push_back (mpd);
    auto descriptor = mpd->descriptor ();
    descriptor.async_wait
      (asio::posix::stream_descriptor::wait_read,
       std::bind (&bash_t::handle_mpd, this, mpd, std::placeholders::_1));
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

  void bash_t::handle_mpd (popen_ptr_t mpd, const asio::error_code &errc)
  {
    if (errc) {
      log_t::buffer_t buf;
      buf << "bash: Failed to handle mpd event";
      log_t::error (buf);
    }

    auto info = mpd->read ();
    if (info.empty () == false)
      m_queue.push (info);

    auto descriptor = mpd->descriptor ();
    descriptor.async_wait
      (asio::posix::stream_descriptor::wait_read,
       std::bind (&bash_t::handle_mpd, this, mpd, std::placeholders::_1));
  }

} // namespace led_d
