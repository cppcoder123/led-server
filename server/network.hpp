//
//
//
#ifndef LED_D_NETWORK_HPP
#define LED_D_NETWORK_HPP

#include "unix/port.hpp"

#include "asio/asio.hpp"

#include "type-def.hpp"

namespace led_d
{
  class network_t
  {

  public:

    network_t (unix::port_t::value_t port,
               asio::io_context &io_context,
               unix_queue_t &queue);
    ~network_t () {};

    void start ();
    void stop ();
    
  private:

    void do_accept ();

    asio::io_context &m_context;
    asio::ip::tcp::acceptor m_acceptor;
    asio::ip::tcp::socket m_socket;

    unix_queue_t &m_queue;
  };
} // namespace led_d

#endif
