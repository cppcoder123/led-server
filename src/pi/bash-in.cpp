//
//
//
#include "unix/log.hpp"

#include "bash-in.hpp"

namespace led_d
{

  constexpr auto mpd_name = "led-mpd.sh";

  bash_in_t::bash_in_t (asio::io_context &io_context, bash_queue_t &queue)
    : m_context (io_context),
      m_queue (queue),
      m_source_vector (SOURCE_SIZE, nullptr)
  {
  }

  void bash_in_t::start ()
  {
    log_t::buffer_t buf;
    buf << "bash-in: Starting service...";
    log_t::info (buf);

    auto source = std::make_shared<popen_t> (mpd_name, m_context, m_queue);
    m_source_vector[MPD] = source;
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

    m_source_vector.clear ();

    // fixme: Do we need to do smth else here?
  }

  bool bash_in_t::kick (source_t source)
  {
    if ((source < 0)
        || (source >= SOURCE_SIZE))
      return false;

    return m_source_vector[source]->kill (SIGUSR1);
  }
} // namespace led_d
