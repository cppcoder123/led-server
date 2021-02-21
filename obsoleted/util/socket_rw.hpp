//
//
//
//
#ifndef UNIX_SOCKET_RW_HPP
#define UNIX_SOCKET_RW_HPP

#include <array>
#include <functional>
#include <string>

#include "asio/asio.hpp"

#include "patch.hpp"
#include "port.hpp"
#include "string-move.hpp"

namespace unix
{

  template <std::size_t buf_size>
  class socket_rw_t
  {

  public:

    // smth is written callback
    using write_t = std::function<void (void)>;

    // smth is read callback
    // Note: User should clear argument!
    using read_t = std::function<void (std::string&)>;

    // smth bad is happened like disconnect
    using disconnect_t = std::function<void (void)>;


    socket_rw_t (asio::ip::tcp::socket &socket,
                 write_t written_cb,
                 read_t read_cb,
                 disconnect_t disconnect_cb);
    ~socket_rw_t () {}

    // Note: socket should be connected before calling these
    void async_read ();
    bool write (const std::string &text);

  private:

    void write_completed (const asio::error_code &error, std::size_t len);
    void read_completed (const asio::error_code &error, std::size_t len);

    write_t m_written;          // write completed
    read_t m_read;              // smth has arrived
    disconnect_t m_disconnect;  // disconnect has occured

    asio::ip::tcp::socket &m_socket;

    bool m_write_started;

    // raw buffers we can pass to asio
    using buffer_t = std::array<char, buf_size>;
    buffer_t m_write_buf;
    buffer_t m_read_buf;
    // rubber buffers
    std::string m_out_buf;
    std::string m_in_buf;
  };

  template <std::size_t size>
  socket_rw_t<size>::socket_rw_t (asio::ip::tcp::socket &socket,
                                  write_t write_cb,
                                  read_t read_cb,
                                  disconnect_t disconnect_cb)
    : m_written (write_cb),
      m_read (read_cb),
      m_disconnect (disconnect_cb),
      m_socket (socket),
      m_write_started (false)
  {
  }

  template <std::size_t size>
  void socket_rw_t<size>::async_read ()
  {
    m_socket.async_read_some
      (asio::buffer (m_read_buf, m_read_buf.size ()),
       std::bind (&socket_rw_t::read_completed, this,
                  std::placeholders::_1, std::placeholders::_2));
  }

  template <std::size_t size>
  bool socket_rw_t<size>::write (const std::string &text)
  {
    if (m_write_started == true)
      return false;

    // 1. rise flag
    m_write_started = true;

    // 2. put into buffer
    auto len = string_move (text, m_write_buf, m_out_buf);

    // 3. start async writing
    asio::async_write
      (m_socket, asio::buffer (m_write_buf.data (), len),
       std::bind (&socket_rw_t::write_completed, this,
                  std::placeholders::_1, std::placeholders::_2));

    return true;
  }

  template <std::size_t size>  void socket_rw_t<size>::
  write_completed (const asio::error_code &error, std::size_t/*not used*/)
  {
    if (error)
      return;

    if (m_out_buf.empty ()) {
      // write is completed
      m_written ();
      m_write_started = false;
      return;
    }

    auto array_size = string_move (m_out_buf, m_write_buf);

    asio::async_write
      (m_socket,
       asio::buffer (m_write_buf.data (), array_size),
       std::bind (&socket_rw_t::write_completed, this,
                  std::placeholders::_1, std::placeholders::_2));
  }

  template <std::size_t size> void socket_rw_t<size>::
  read_completed (const asio::error_code &error, std::size_t len)
  {
    if (error) {
      if ((error == asio::error::eof)
          || (error == asio::error::connection_reset))
        m_disconnect ();
      return;
    }

    string_move_append (m_read_buf, len, m_in_buf);

    m_read (m_in_buf);

    async_read ();
  }

  //
  // Keep following code a bit
  //
#if 0
  inline bool socket_rw_t::send (const request_t &request)
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

  inline bool socket_rw_t::write (const request_t &request, asio::ip::tcp::socket &socket)
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

  inline bool socket_rw_t::read (asio::ip::tcp::socket &socket)
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
