//
//
//
//
#ifndef UNIX_CLIENT_HPP
#define UNIX_CLIENT_HPP

#include <array>
#include <bitset>
#include <functional>
#include <string>

#include "asio/asio.hpp"

#include "patch.hpp"
#include "port.hpp"
#include "socket_rw.hpp"

namespace unix
{

  template <std::size_t buf_size>
  class client_t
  {

  public:

    using connect_t = std::function<void (void)>;
    using write_t = typename socket_rw_t<buf_size>::write_t;
    using read_t = typename socket_rw_t<buf_size>::read_t;
    using disconnect_t = typename socket_rw_t<buf_size>::disconnect_t;

    client_t (asio::io_context &context,
              port_t::value_t port,
              const std::string &host,
              connect_t connected_cb,
              write_t written_cb,
              read_t read_cb,
              disconnect_t disconnect_cb);
    ~client_t () {}

    // connect and handle connection then
    void connect (bool force = false);
    bool write (const std::string &text);

  private:

    void connect_completed (const asio::error_code &error);
    void disconnect_completed ();

    asio::io_context &m_context;
    port_t::value_t m_port;
    const std::string m_host;

    connect_t m_connected;
    disconnect_t m_disconnected;

    asio::ip::tcp::socket m_socket;

    enum {
      CONNECT_STARTED,
      CONNECTED,
      SIZE
    };
    std::bitset<SIZE> m_flag;

    //static constexpr std::size_t m_connect_delay = 1; // second
    asio::steady_timer m_connect_timer;

    socket_rw_t<buf_size> m_rw;
  };

  template <std::size_t size>
  client_t<size>::client_t (asio::io_context &context,
                            port_t::value_t port,
                            const std::string &host,
                            connect_t connected_cb,
                            write_t written_cb,
                            read_t read_cb,
                            disconnect_t disconnect_cb)
    : m_context (context),
      m_port (port),
      m_host (host),
      m_connected (connected_cb),
      m_socket (m_context),
      m_connect_timer (m_context),
      m_rw (m_socket, written_cb, read_cb,
            std::bind (&client_t::disconnect_completed, this))
  {
  }

  template <std::size_t size>
  void client_t<size>::connect (bool force)
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
       (&client_t::connect_completed, this, std::placeholders::_1));
  }

  template <std::size_t size>
  bool client_t<size>::write (const std::string &text)
  {
    if (m_flag.test (CONNECTED) == false)
      return false;

    return m_rw.write (text);
  }

  template <std::size_t size>
  void client_t<size>::connect_completed (const asio::error_code &error)
  {
    if (error) {
      m_connect_timer.expires_at (std::chrono::steady_clock::now ()
                                  + std::chrono::seconds (1/*m_connect_delay*/));
      m_connect_timer.async_wait
        (std::bind (&client_t::connect, this, true));
      return;
    }

    m_connected ();

    m_rw.async_read ();

    m_flag.reset (CONNECT_STARTED);
    m_flag.set (CONNECTED);
  }

  template <std::size_t size>
  void client_t<size>::disconnect_completed ()
  {
    m_flag.reset (CONNECTED);
    m_flag.reset (CONNECT_STARTED); // ?

    m_disconnected ();
  }

  //
  // Keep following code a bit
  //
#if 0
  inline bool client_t::send (const request_t &request)
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

  inline bool client_t::write (const request_t &request, asio::ip::tcp::socket &socket)
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

  inline bool client_t::read (asio::ip::tcp::socket &socket)
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

}// namespace unix

#endif
