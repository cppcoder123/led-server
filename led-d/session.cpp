//
//
//

#include "libled/codec.hpp"
#include "libled/refsymbol.hpp"
#include "libled/request.hpp"


#include "log-wrapper.hpp"
#include "queue.hpp"
#include "message.hpp"
#include "message-ptr.hpp"
#include "session.hpp"

namespace led_d
{

  void session_t::start ()
  {
    do_read ();
  }

  void session_t::send (const std::string &info)
  {
    bool kick_required (false);

    {
      guard_t g (m_write_mutex);
      if (m_write_buf.empty ()) {
        m_write_buf = info;
        kick_required = true;
      } else {
        m_write_buf += info;
      }
    }

    // mutex should be released
    if (kick_required == true)
      do_write ();
  }

  void session_t::do_read ()
  {
    typedef libled::codec_t<libled::refsymbol_t, libled::request_t> codec_t;
    
    session_ptr_t session_ptr (shared_from_this ());
    m_socket.async_read_some
      (asio::buffer (m_raw_read_buf, m_max_size),
       [this, session_ptr] (std::error_code code, std::size_t length)
       {
         if (code) {
           log_t::error ("Failed to read info from socket");
           return;
         }
         m_raw_read_buf[length] = 0;
         m_read_buf += m_raw_read_buf;
         std::string current_msg, next_msg;
         //
         while (codec_t::decode (m_read_buf, current_msg, next_msg) == true) {
           (queue_buffer ()).push
             (message_ptr_t (new message_t (current_msg, session_ptr)));
           m_read_buf = next_msg;
         }
         if (m_read_buf.empty () == false)
           // incompleted message, continue reading
           do_read ();
       }
       );
  }

  void session_t::do_write ()
  {
    std::size_t size (0);
    {
      guard_t g (m_write_mutex);
      if (m_raw_write_buf[0] == 0) { // otherwise we are writing
        if (m_write_buf.size () > m_max_size) {
          m_write_buf.copy (m_raw_write_buf, m_max_size);
          m_write_buf = m_write_buf.substr (m_max_size);
          size = m_max_size;
        } else {
          m_write_buf.copy (m_raw_write_buf, m_write_buf.size ());
          size = m_write_buf.size ();
          m_write_buf.clear ();
        }
      }
    }

    session_ptr_t session_ptr (shared_from_this ());
    asio::async_write (m_socket, asio::buffer (m_raw_write_buf, size),
                       [this, session_ptr, size] (std::error_code ec, std::size_t len)
                       {
                         if (ec) {
                           log_t::error ("Error during socket writing");
                           return;
                         }
                         bool continue_write = true;
                         {
                           guard_t g (m_write_mutex);
                           if (size > len) {
                             std::string tmp (m_raw_write_buf + len, size - len);
                             tmp.copy (m_raw_write_buf, tmp.size ()); // we can't get long string here
                           } else if (size == len) {
                             m_raw_write_buf[0] = 0;
                             if (m_write_buf.empty () == true)
                               continue_write = false;
                           }
                         }
                         if (continue_write == true)
                           do_write ();
                       });
  }

  
} // namespace led_d
