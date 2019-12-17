/*
 *
 */

#include "bash-out.hpp"

namespace led_d
{

  bash_out_t::bash_out_t (status_queue_t &status_queue)
    : m_status_queue (status_queue),
      m_go (true)
  {
  }

  void bash_out_t::start ()
  {
    while (m_go.load () == true) {
      auto command = m_command_queue.pop<true>();
      if (!command)
        continue;
      invoke_command (*command);
    }
  }

  void bash_out_t::stop ()
  {
    m_go.store (false);

    m_command_queue.notify_one<true> ();
  }
  
} // led_d
