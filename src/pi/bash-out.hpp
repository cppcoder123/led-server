/*
 *
 */

#ifndef BASH_OUT_HPP
#define BASH_OUT_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "command-queue.hpp"
#include "status-queue.hpp"

namespace led_d
{

  class bash_out_t
  {
  public:
    bash_out_t (status_queue_t &status_queue);
    ~bash_out_t () = default;

    void start ();
    void stop ();

    command_queue_t& command_queue () {return m_command_queue;}

  private:

    void invoke_command (command_t command);

    status_queue_t &m_status_queue;

    command_queue_t m_command_queue;

    std::atomic_bool m_go;

  };

} // led_d

#endif

