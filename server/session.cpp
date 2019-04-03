//
//
//

#include "unix/codec.hpp"
#include "unix/log.hpp"
#include "unix/refsymbol.hpp"
#include "unix/request.hpp"

#include "session.hpp"
#include "type-def.hpp"

namespace led_d
{

  session_t::session_t (asio::ip::tcp::socket socket, unix_queue_t &queue)
    : m_socket (std::move (socket)),
      m_queue (queue),
      m_rw (m_socket,
            std::bind (&session_t::write_cb, this),
            std::bind (&session_t::read_cb, this, std::placeholders::_1),
            std::bind (&session_t::disconnect_cb, this)),
      m_disconnect ([](){})
  {
  }

  session_t::~session_t ()
  {
    log_t::info ("Session destructor is called");
  }

  void session_t::start ()
  {
    m_rw.async_read ();
  }

  void session_t::write (const std::string &info)
  {
    if ((m_postponed_write.empty () == true)
        && (m_rw.write (info) == true))
      return;

    m_postponed_write.push_back (info);
  }

  void session_t::set_disconnect (disconnect_t cb)
  {
    m_disconnect = cb;
  }

  void session_t::write_cb ()
  {
    if (m_postponed_write.empty () == true)
      return;

    std::string info = m_postponed_write.front ();
    m_postponed_write.pop_front ();

    if (m_rw.write (info) == false)
      log_t::error ("session: Failed to write in write_cb");
  }

  void session_t::read_cb (std::string &info)
  {
    using codec_t = unix::codec_t<unix::refsymbol_t>;

    std::size_t text_len = 0;
    while (codec_t::decode<unix::request_t> (info, text_len) == true) {
      std::string text = info.substr (0, text_len);
      info = info.substr(text_len);
      m_queue.push
        (std::make_unique<unix_msg_t>(text, shared_from_this ()));
    }
  }

  void session_t::disconnect_cb ()
  {
    m_disconnect ();
  }

} // namespace led_d
