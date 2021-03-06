//
//
//
#include <functional>

#include "unix/log.hpp"

#include "bash-in.hpp"

namespace led_d
{

  constexpr auto mpd_name = "led-mpd.sh";
  constexpr auto sys_name = "led-sys.sh";

  bash_in_t::bash_in_t (asio::io_context &io_context, bash_queue_t &queue)
    : m_context (io_context),
      m_queue (queue)
      // m_source_vector (SOURCE_SIZE, nullptr)
  {
  }

  void bash_in_t::start ()
  {
    log_t::buffer_t buf;
    buf << "bash-in: Starting service...";
    log_t::info (buf);

    auto in = std::make_shared<popen_t>
      (mpd_name, m_context,
       std::bind (&bash_in_t::info_arrived, this, std::placeholders::_1),
       std::bind (&bash_in_t::error_occurred, this, mpd_name));
    m_in_list.push_back (in);

    in = std::make_shared<popen_t>
      (sys_name, m_context,
       std::bind (&bash_in_t::info_arrived, this, std::placeholders::_1),
       std::bind (&bash_in_t::error_occurred, this, sys_name));
    m_in_list.push_back (in);
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

    m_in_list.clear ();

    // fixme: Do we need to do smth else here?
  }

  void bash_in_t::info_arrived (const std::string &info)
  {
    m_queue.push (info);
  }

  void bash_in_t::error_occurred (const char *where)
  {
    log_t::buffer_t buf;
    buf << "bash-in: Error occured while handling \"" << where << "\"";
    log_t::error (buf);
  }

  // bool bash_in_t::kick (source_t source)
  // {
  //   if ((source < 0)
  //       || (source >= SOURCE_SIZE))
  //     return false;

  //   return m_source_vector[source]->kill (SIGUSR1);
  // }
} // namespace led_d
