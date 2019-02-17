//
//
//
#ifndef UNIX_ASYNC_CLIENT_HPP
#define UNIX_ASYNC_CLIENT_HPP

#include <array>
#include <bitset>
#include <functional>
#include <string>

#include "asio/asio.hpp"

#include "log.hpp"
#include "codec.hpp"
#include "final-action.hpp"
#include "patch.hpp"
#include "port.hpp"
#include "refsymbol.hpp"
#include "request.hpp"
#include "response.hpp"
#include "string-move.hpp"

namespace unix
{

  class async_client_t
  {

  public:

    using notify_t = std::function<void (void)>;
    using receive_t = std::function<void (const response_t&)>;
    
    async_client_t (asio::io_context &context,
                    port_t::value_t port,
                    const std::string &host,
                    notify_t connected,
                    notify_t written,
                    receive_t receive);
    ~async_client_t () {}

    // connect and handle connection then
    void connect (bool force = false);
    bool send (const request_t &request);

  private:

    void connect_completed (const asio::error_code &error);
    void write_completed (const asio::error_code &error, std::size_t len);
    void read_completed (const asio::error_code &error, std::size_t len);

    asio::io_context &m_context;
    port_t::value_t m_port;
    const std::string m_host;

    notify_t m_connected;
    notify_t m_written;
    receive_t m_receive;        // smth has arrived

    asio::ip::tcp::socket m_socket;

    enum {
      CONNECT_STARTED,
      CONNECTED,
      WRITE_STARTED,            // trying to write
      SIZE
    };
    std::bitset<SIZE> m_flag;

    // buffers
    static const std::size_t m_buf_size = 255;
    // raw buffers we can pass to asio
    using buffer_t = std::array<char, m_buf_size>;
    buffer_t m_write_buf;
    buffer_t m_read_buf;
    // rubber buffers
    std::string m_out_buf;
    std::string m_in_buf;

    static constexpr std::size_t m_connect_delay = 1; // second
    asio::steady_timer m_connect_timer;
  };

  inline async_client_t::async_client_t (asio::io_context &context,
                                         port_t::value_t port,
                                         const std::string &host,
                                         notify_t connected,
                                         notify_t written,
                                         receive_t receive)
    : m_context (context),
      m_port (port),
      m_host (host),
      m_connected (connected),
      m_written (written),
      m_receive (receive),
      m_socket (m_context),
      m_connect_timer (m_context)
  {
  }

  inline void async_client_t::connect (bool force)
  {
    if ((m_flag.test (CONNECTED) == true)
        || ((force == false)
            && (m_flag.test (CONNECT_STARTED) == true)))
      return;

    m_flag.set (CONNECT_STARTED);

    static asio::ip::tcp::resolver resolver (m_context);
    std::string port_string (patch::to_string (m_port));
    static asio::ip::tcp::resolver::query
      query (asio::ip::tcp::v4 (), m_host, port_string);
    static auto iterator = resolver.resolve (query);

    asio::async_connect
      (m_socket, iterator,
       std::bind
       (&async_client_t::connect_completed, this, std::placeholders::_1));
  }

  inline bool async_client_t::send (const request_t &request)
  {
    if ((m_flag.test (CONNECTED) == false)
        || (m_flag.test (WRITE_STARTED) == true))
      return false;

    // 1. encode
    using codec_t = unix::codec_t<refsymbol_t, request_t>;
    std::string msg;
    if (codec_t::encode (request, msg) == false) {
      log_t::error ("Failed to encode request");
      return false;
    }

    m_flag.set (WRITE_STARTED);

    // 2. put into buffer
    auto len = string_move (msg, m_write_buf, m_out_buf);

    // 3. start async writing
    asio::async_write
      (m_socket, asio::buffer (m_write_buf.data (), len),
       std::bind (&async_client_t::write_completed, this,
                  std::placeholders::_1, std::placeholders::_2));

    return true;
  }

#if 0
  inline bool async_client_t::send (const request_t &request)
  {
    asio::io_service io_service;

    asio::ip::tcp::resolver resolver (io_service);
    std::string port_string (patch::to_string (m_port));
    asio::ip::tcp::resolver::query
      query (asio::ip::tcp::v4 (), m_host, port_string);
    asio::ip::tcp::resolver::iterator iterator = resolver.resolve (query);

    asio::ip::tcp::socket socket (io_service);
    asio::connect (socket, iterator);

    if (write (request, socket) == false)
      return false;

    return read (socket);
  }

  inline bool async_client_t::write (const request_t &request, asio::ip::tcp::socket &socket)
  {
    using codec_t = unix::codec_t<refsymbol_t, request_t>;
    std::string msg;
    if (codec_t::encode (request, msg) == false) {
      log_t::error ("Failed to encode request");
      return false;
    }

    asio::write (socket, asio::buffer (msg.c_str (), msg.size ()));

    return true;
  }

  inline bool async_client_t::read (asio::ip::tcp::socket &socket)
  {
    using codec_t = unix::codec_t<refsymbol_t, response_t>;
    char response_buf[codec_t::max_size];
    std::size_t response_size =
      asio::read (socket, asio::buffer (response_buf, codec_t::header_size));
    std::size_t msg_size (0);
    if (codec_t::decode
        (std::string (response_buf, response_size), msg_size) == false) {
      log_t::error ("Failed to decode header");
      return false;
    }

      response_size =
        asio::read (socket, asio::buffer (response_buf, msg_size));
      response_t response;
      if (codec_t::decode (std::string (response_buf,
                                        response_size), response) == false) {
        log_t::error ("Failed to decode reply");
        return false;
      }
      if (response.status != response_t::status_ok) {
        log_t::buffer_t buf;
        buf << "Error in response : " << response.reason;
        log_t::error (buf);
        return false;
      }

      //std::cout << "Good response : " << response.reason << "\n";
      return true;
  }
#endif

  inline void async_client_t::
  connect_completed (const asio::error_code &error)
  {
    if (error) {
      m_connect_timer.expires_at (std::chrono::steady_clock::now ()
                                  + std::chrono::seconds (m_connect_delay));
      m_connect_timer.async_wait
        (std::bind (&async_client_t::connect, this, true));
      return;
    }

    m_connected ();

    // we can expect messages now
    asio::async_read
      (m_socket, asio::buffer (m_read_buf, m_read_buf.size ()),
       std::bind (&async_client_t::read_completed, this,
                  std::placeholders::_1, std::placeholders::_2));

    m_flag.reset (CONNECT_STARTED);
    m_flag.set (CONNECTED);
  }

  inline void async_client_t::
  write_completed (const asio::error_code &error, std::size_t/*not used*/)
  {
    if (error)
      return;

    if (m_out_buf.empty ()) {
      // write is completed
      m_written ();
      m_flag.reset (WRITE_STARTED);
      return;
    }

    auto array_size = string_move (m_out_buf, m_write_buf);

    asio::async_write
      (m_socket,
       asio::buffer (m_write_buf.data (), array_size),
       std::bind (&async_client_t::write_completed, this,
                  std::placeholders::_1, std::placeholders::_2));
  }
  
  inline void async_client_t::
  read_completed (const asio::error_code &error, std::size_t len)
  {
    if (error)
      return;

    string_move_append (m_read_buf, len, m_in_buf);

    using codec_t = unix::codec_t<refsymbol_t, response_t>;

    // 1. decode header
    if ((codec_t::decode (m_in_buf, len) == false) // re-use len
        || (m_in_buf.size () < len))
      return;

    // we need to clear buf sometimes
    final_action_t at_exit
      = make_final_action ([this](){m_in_buf.clear ();});

    response_t response;
    if (codec_t::decode (m_in_buf, response) == false) {
        log_t::error ("Failed to decode reply");
        return;
    }

    m_receive (response);
  }

  
}// namespace unix

#endif
