//
//
//
// To do: handle priority properly
//

#include "unix/codec.hpp"
#include "unix/refsymbol.hpp"

#include "daemon.hpp"
#include "init.hpp"
#include "log-wrapper.hpp"

namespace led_info_d
{
  daemon_t::daemon_t (const arg_t &arg)
    : m_timer (m_context),
      m_client (m_context,
                arg.port,
                arg.host,
                std::bind (&daemon_t::notify_connect, this),
                std::bind (&daemon_t::notify_write, this),
                std::bind (&daemon_t::notify_read, this, std::placeholders::_1))
  {
  }

  int daemon_t::start ()
  {
    //m_schedule.start ();
    m_client.connect ();
    return 0;
  }

  void daemon_t::stop ()
  {
    //m_schedule.stop ();
    //
    for (map_t::iterator iter = m_map.begin (); iter != m_map.end (); ++iter) {
      request_ptr_t request_ptr (iter->second);
      request_ptr->action = unix::request_t::action_erase;
      write (*request_ptr);
    }
  }

  void daemon_t::schedule (const delay_t &delay, callback_t cb)
  {
    m_timer.expires_at (std::chrono::steady_clock::now () + delay);
    m_timer.async_wait (cb);
  }

  void daemon_t::info (priority_id_t prio,
                       const unix::request_t &request)
  {
    map_t::iterator iter (m_map.find (request.tag));
    if (request.action == unix::request_t::action_insert) {
      request_ptr_t request_ptr (std::make_unique<unix::request_t>(request));
      m_map[request.tag] = request_ptr;
    } else if (request.action == unix::request_t::action_erase) {
      if (iter != m_map.end ())
        m_map.erase (iter);
    }

    write (request);
  }

  void daemon_t::notify_connect ()
  {
    log_t::buffer_t buf;
    buf << "daemon: Gateway is connected to Led server";
    log_t::info (buf);

    init (*this);
  }

  void daemon_t::notify_write ()
  {
    if (m_write_queue.empty () == true)
      return;

    auto msg = m_write_queue.front ();
    m_write_queue.pop_front ();

    if (m_client.write (msg) == false) {
      log_t::buffer_t buf;
      buf << "daemon: notify_write - internal error, we shouldn't be here";
      log_t::error (buf);
    }
  }

  void daemon_t::notify_read (std::string &in)
  {
    // fixme
    // make it empty now
    in = "";

    log_t::buffer_t buf;
    buf << "daemon: notify_read - not implemented";
    log_t::error (buf);
  }

  void daemon_t::write (const unix::request_t &request)
  {
    using codec_t = unix::codec_t<unix::refsymbol_t, unix::request_t>;

    std::string msg;
    if (codec_t::encode (request, msg) == false) {
      log_t::error ("Failed to encode request");
      return;
    }

    if (m_write_queue.empty () == false) {
      m_write_queue.emplace_back (msg);
      return;
    }

    if (m_client.write (msg) == false)
      m_write_queue.emplace_back (msg);
  }

} // namespace led_info_d
