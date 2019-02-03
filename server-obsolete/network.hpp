//
//
//
#ifndef LED_D_NETWORK_HPP
#define LED_D_NETWORK_HPP

#include "asio/asio.hpp"

#include "port.hpp"

#include "message-queue.hpp"

namespace led_d
{
  class network_t
  {

  public:

    network_t (asio::io_service &io_service,
               core::port_t::value_t port,
               queue_t &queue);
    ~network_t () {};

  private:

    void do_accept ();

    asio::ip::tcp::acceptor m_acceptor;
    asio::ip::tcp::socket m_socket;

    queue_t &m_queue;
  };
} // namespace led_d

#endif
