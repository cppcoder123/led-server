/*
 *
 */

#include "unix/log.hpp"

#include "bash-out.hpp"
#include "popen.hpp"

namespace led_d
{

  bash_out_t::bash_out_t (asio::io_context &io_context,
                          status_queue_t &status_queue)
    : m_io_context (io_context),
      m_status_queue (status_queue),
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

  void bash_out_t::invoke_command (command_ptr_t command)
  {
    auto body = (command->wrap ())
      ? wrap (command->body ()) : command->body ();
    auto info_cb = (command->stream ())
      ? std::bind (&bash_out_t::stream_info, this, std::placeholders::_1)
      : std::bind (&bash_out_t::clot_info, this, std::placeholders::_1);
    auto error_cb = (command->stream ())
      ? std::bind (&bash_out_t::stream_error, this, command)
      : std::bind (&bash_out_t::clot_error, this, command);
    auto popen = std::make_shared<popen_t>
      (body, m_io_context, info_cb, error_cb);
    command->popen (popen);

    insert (command);
  }

} // led_d
