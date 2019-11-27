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

    auto popen = std::make_shared<popen_t>(mpd_query_name, true, m_context);
    m_popen_list.push_back (popen);
    auto &descriptor = popen->descriptor ();
    descriptor.async_wait
      (asio::posix::stream_descriptor::wait_read,
       std::bind (&bash_in_t::handle_popen, this, popen, std::placeholders::_1));
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

    // fixme: Do we need to do smth else here?
  }

  void bash_in_t::handle_popen (popen_ptr_t popen, const asio::error_code &errc)
  {
    if (errc) {
      log_t::buffer_t buf;
      buf << "bash-in: Failed to handle popen event";
      log_t::error (buf);
    }

    std::string info;
    while (popen->read (info) == true)
      if (info.empty () == false)
        m_queue.push (info);

    auto &descriptor = popen->descriptor ();
    descriptor.async_wait
      (asio::posix::stream_descriptor::wait_read,
       std::bind (&bash_in_t::handle_popen, this, popen, std::placeholders::_1));
  }

} // namespace led_d
