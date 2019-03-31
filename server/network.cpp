//
//
//
#include "log-wrapper.hpp"
#include "network.hpp"
#include "session.hpp"

namespace led_d
{

  network_t::network_t (unix::port_t::value_t port,
                        asio::io_context &io_context,
                        unix_queue_t &queue)
    : m_context (io_context),
      m_acceptor (m_context, asio::ip::tcp::endpoint (asio::ip::tcp::v4 (), port)),
      m_socket (io_context),
      m_queue (queue)
  {
    do_accept ();
  }

  void network_t::start ()
  {
    log_t::buffer_t buf;
    buf << "network: Starting service...";
    log_t::info (buf);
  }

  void network_t::stop ()
  {
    log_t::buffer_t buf;
    buf << "network: Stopping the service...";
    log_t::info (buf);

    m_context.stop ();

    log_t::clear (buf);
    buf << "network: Service is stopped";
    log_t::info (buf);

    // fixme: Do we need to do smth else here?
  }

  void network_t::do_accept ()
  {
    m_acceptor.async_accept
      (m_socket,
       [this] (std::error_code err_code)
       {
         if (!err_code) {
           m_session = std::make_shared<session_t>(std::move (m_socket), m_queue);
           m_session->start ();
         }
         do_accept ();
       }
       );
  }

} // namespace led_d
