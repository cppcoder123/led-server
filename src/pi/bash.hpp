//
//
//
#ifndef BASH_HPP
#define BASH_HPP

#include "asio/asio.hpp"

#include "bash-queue.hpp"

namespace led_d
{
  class bash_t
  {

  public:

    bash_t (asio::io_context &io_context, bash_queue_t &queue);
    ~bash_t () {};

    void start ();
    void stop ();

  private:

    // void do_accept ();

    asio::io_context &m_context;

    bash_queue_t &m_queue;

    // std::shared_ptr<session_t> m_session;
  };
} // namespace led_d

#endif
