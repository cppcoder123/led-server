//
//
//
#ifndef LIBLED_CLIENT_HPP
#define LIBLED_CLIENT_HPP

#include <string>

#include "asio.hpp"

#include "libled/codec.hpp"
#include "libled/log.hpp"
#include "libled/patch.hpp"
#include "libled/port.hpp"
#include "libled/refsymbol.hpp"
#include "libled/request.hpp"
#include "libled/response.hpp"

namespace libled
{

  class client_t
  {

  public:

    client_t (port_t::value_t port, const std::string &host)
      : m_port (port),
        m_host (host) {}
    ~client_t () {}

    bool send (const request_t &request);

  private:

    inline bool write (const request_t &request, asio::ip::tcp::socket &socket);
    inline bool read (asio::ip::tcp::socket &socket);
    
    port_t::value_t m_port;
    const std::string m_host;
  };


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
    typedef codec_t<refsymbol_t, request_t> codec_t;
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
    typedef codec_t<refsymbol_t, response_t> codec_t;
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
  
}// namespace libled

#endif
