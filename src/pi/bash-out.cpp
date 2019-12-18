/*
 *
 */

#include "unix/log.hpp"

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
      invoke_command (std::move(*command));
    }
  }

  void bash_out_t::stop ()
  {
    m_go.store (false);

    m_command_queue.notify_one<true> ();
  }

#if 0
  popen should be used here
  void bash_out_t::invoke_command (command_t command)
  {
    auto cmd = command.body ();
    if (body.empty ()) {
      log_t::error ("bash-out: Empty command body");
      return;
    }

    auto code = std::system (cmd.c_str ());

    if (command.simple () == true) {
      
      return;
    }
  }
#endif

} // led_d
