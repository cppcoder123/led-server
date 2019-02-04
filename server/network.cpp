//
//
//
#include "network.hpp"
#include "session.hpp"

namespace led_d
{

  network_t::network_t (asio::io_service &io_service,
                        core::port_t::value_t port,
                        queue_t &queue)
    : m_acceptor (io_service, asio::ip::tcp::endpoint (asio::ip::tcp::v4 (), port)),
      m_socket (io_service),
      m_queue (queue)
  {
    do_accept ();
  }

  void network_t::do_accept ()
  {
    m_acceptor.async_accept
      (m_socket,
       [this] (std::error_code err_code)
       {
         if (!err_code) {
           std::make_shared<session_t>(std::move (m_socket), m_queue)->start ();
         }
         do_accept ();
       }
       );
  }

} // namespace led_d
