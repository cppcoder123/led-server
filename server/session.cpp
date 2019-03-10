//
//
//

#include "unix/codec.hpp"
#include "unix/refsymbol.hpp"
#include "unix/request.hpp"


#include "log-wrapper.hpp"
//#include "message.hpp"
//#include "message-ptr.hpp"
#include "session.hpp"
#include "type-def.hpp"

namespace led_d
{

  session_t::session_t (asio::ip::tcp::socket socket, unix_queue_t &queue)
    : m_socket (std::move (socket)),
      m_queue (queue),
      m_rw (m_socket,
            std::bind (&session_t::write_cb, shared_from_this ()),
            std::bind (&session_t::read_cb, shared_from_this (), std::placeholders::_1))
  {
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
    using codec_t = unix::codec_t<unix::refsymbol_t, unix::request_t>;

    std::size_t text_len;
    while (codec_t::decode (info, text_len) == true) {
      std::string text = info.substr (0, text_len);
      info = info.substr(text_len);
      m_queue.push
        (std::make_unique<unix_msg_t>(text, shared_from_this ()));
    }
  }

  //   session_ptr_t session_ptr (shared_from_this ());
  //   m_socket.async_read_some
  //     (asio::buffer (m_raw_read_buf, m_max_size),
  //      [this, session_ptr] (std::error_code code, std::size_t length)
  //      {
  //        if (code) {
  //          log_t::error ("Failed to read info from socket");
  //          return;
  //        }
  //        m_raw_read_buf[length] = 0;
  //        m_read_buf += m_raw_read_buf;
  //        std::string current_msg, next_msg;
  //        //
  //        while (codec_t::decode (m_read_buf, current_msg, next_msg) == true) {
  //          m_queue.push
  //            (std::make_unique<unix_msg_t>(current_msg, session_ptr));
  //          m_read_buf = next_msg;
  //        }
  //        if (m_read_buf.empty () == false)
  //          // incomplete message, continue reading
  //          do_read ();
  //      }
  //      );
  // }

  // void session_t::do_write ()
  // {
  //   std::size_t size (0);
  //   {
  //     guard_t g (m_write_mutex);
  //     if (m_raw_write_buf[0] == 0) { // otherwise we are writing
  //       if (m_write_buf.size () > m_max_size) {
  //         m_write_buf.copy (m_raw_write_buf, m_max_size);
  //         m_write_buf = m_write_buf.substr (m_max_size);
  //         size = m_max_size;
  //       } else {
  //         m_write_buf.copy (m_raw_write_buf, m_write_buf.size ());
  //         size = m_write_buf.size ();
  //         m_write_buf.clear ();
  //       }
  //     }
  //   }

  //   session_ptr_t session_ptr (shared_from_this ());
  //   asio::async_write (m_socket, asio::buffer (m_raw_write_buf, size),
  //     [this, session_ptr, size] (std::error_code ec, std::size_t len)
  //     {
  //       if (ec) {
  //         log_t::error ("Error during socket writing");
  //         return;
  //       }
  //       bool continue_write = true;
  //       {
  //         guard_t g (m_write_mutex);
  //         if (size > len) {
  //           std::string tmp (m_raw_write_buf + len, size - len);
  //           tmp.copy (m_raw_write_buf, tmp.size ()); // we can't get long string here
  //         } else if (size == len) {
  //           m_raw_write_buf[0] = 0;
  //           if (m_write_buf.empty () == true)
  //             continue_write = false;
  //         }
  //       }
  //       if (continue_write == true)
  //         do_write ();
  //     });
  // }

} // namespace led_d
