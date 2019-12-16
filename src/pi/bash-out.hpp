/*
 *
 */

#ifndef BASH_OUT_HPP
#define BASH_OUT_HPP

#include "command-queue.hpp"
#include "status-queue.hpp"

namespace led_d
{

  class bash_out_t
  {
  public:
    bash_out_t (status_queue_t &status_queue);
    ~bash_out_t ();

    void start ();
    void stop ();

    command_queue_t& command_queue () {return m_command_queue;}

  private:

    command_queue_t m_command_queue;
    status_queue_t &m_status_queue;

  };

} // led_d

#endif

