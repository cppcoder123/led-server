//
//
//
#include "unix/log.hpp"

#include "bash.hpp"

namespace led_d
{

  bash_t::bash_t (asio::io_context &io_context,
                  bash_queue_t &queue)
    : m_context (io_context),
      m_queue (queue)
  {
  }

  void bash_t::start ()
  {
    log_t::buffer_t buf;
    buf << "bash: Starting service...";
    log_t::info (buf);
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

} // namespace led_d
