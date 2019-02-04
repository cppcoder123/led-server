//
//
//
#ifndef LED_D_NETWORK_HPP
#define LED_D_NETWORK_HPP

#include "asio/asio.hpp"

//#include "port.hpp"

#include "message-queue.hpp"

namespace led_d
{
  class network_t
  {

  public:

    network_t (asio::io_context &io_context,
               message_queue_t &queue);
    ~network_t () {};

    void start (/*pass arg_t here*/);
    void stop ();
    
  private:

    void do_accept ();

    asio::ip::tcp::acceptor m_acceptor;
    asio::ip::tcp::socket m_socket;

    message_queue_t &m_queue;
  };
} // namespace led_d

#endif
