//
//
//
// To do: handle priority properly
//

#include "unix/codec.hpp"
#include "unix/log.hpp"
#include "unix/refsymbol.hpp"

#include "daemon.hpp"

namespace led_info_d
{
  daemon_t::daemon_t (const arg_t &arg, asio::io_context &context)
    : m_client (context,
                arg.port,
                arg.host,
                std::bind (&daemon_t::notify_connect, this),
                std::bind (&daemon_t::notify_write, this),
                std::bind (&daemon_t::notify_read, this, std::placeholders::_1),
                std::bind (&daemon_t::notify_disconnect, this)),
      m_content (context,
                 std::bind (&daemon_t::write, this, std::placeholders::_1))
  {
  }

  bool daemon_t::start ()
  {
    m_content.init ();

    m_client.connect ();

    return true;
  }

  void daemon_t::stop ()
  {
  }

  void daemon_t::notify_connect ()
  {
    log_t::buffer_t buf;
    buf << "daemon: Gateway is connected to Led server";
    log_t::info (buf);

    request_t request;
    request.action = request_t::subscribe;
    write (request);
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
    using codec_t = unix::codec_t<refsymbol_t>;

    std::size_t len = 0;
    if ((codec_t::decode (in, len) == false)
        || (in.size () < len))
      // msg is not arrived
      return;

    std::string tmp = in.substr (0, len);
    in = in.substr(len);

    response_t response;
    if (codec_t::decode (tmp, response) == false) {
      log_t::error ("Failed to decode response");
      return;
    }

    switch (response.status) {
    case response_t::ok:
      // do nothing
      break;
    case response_t::error:
        log_t::error ("Error status has arrived!");
      break;
    case response_t::poll:
      m_content.push ();
      break;
    case response_t::button:
      log_t::error ("Button response has arrived, but not implemented!");
      break;
    default:
      log_t::error ("Unknown response has arrived");
      break;
    }
  }

  void daemon_t::notify_disconnect ()
  {
    log_t::error ("Connection is lost, trying to restore it!");
    m_client.connect ();
  }

  void daemon_t::write (const request_t &request)
  {
    using codec_t = unix::codec_t<refsymbol_t>;

    std::string msg;
    if (codec_t::encode (request, msg) == false) {
      log_t::error ("Failed to encode request");
      return;
    }

    if ((m_write_queue.empty () == true)
        && (m_client.write (msg) == true))
      return;

    m_write_queue.emplace_back (msg);
  }

} // namespace led_info_d
