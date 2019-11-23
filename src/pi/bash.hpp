//
//
//
#ifndef BASH_HPP
#define BASH_HPP

#include "unix/port.hpp"

#include "asio/asio.hpp"

#include "bash-queue.hpp"

namespace led_d
{
  class bash_t
  {

  public:

    bash_t (unix::port_t::value_t port,
               asio::io_context &io_context,
               bash_queue_t &queue);
    ~bash_t () {};

    void start ();
    void stop ();

  private:

    void do_accept ();

    asio::io_context &m_context;
    asio::ip::tcp::acceptor m_acceptor;
    asio::ip::tcp::socket m_socket;

    bash_queue_t &m_queue;

    std::shared_ptr<session_t> m_session;
  };
} // namespace led_d

#endif
