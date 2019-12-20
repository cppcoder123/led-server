/*
 *
 */

#include <chrono>
#include <functional>

#include "unix/log.hpp"

#include "bash-out.hpp"
#include "popen.hpp"

namespace led_d
{

  bash_out_t::bash_out_t (asio::io_context &io_context,
                          status_queue_t &status_queue)
    : m_io_context (io_context),
      m_timeout_timer (m_io_context),
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
      ? std::bind
      (&bash_out_t::stream_info, this, command, std::placeholders::_1)
      : std::bind
      (&bash_out_t::clot_info, this, command, std::placeholders::_1);

    auto error_cb = (command->stream ())
      ? std::bind (&bash_out_t::stream_error, this, command)
      : std::bind (&bash_out_t::clot_error, this, command);

    m_timeout_timer.expires_from_now
      (std::chrono::seconds (command->timeout ()));
    m_timeout_timer.async_wait
      (std::bind (&bash_out_t::timeout, this, command));

    // insert cmd before creating popen
    insert (command);

    auto popen = std::make_shared<popen_t>
      (body, m_io_context, info_cb, error_cb);
    command->popen (popen);
  }

  std::string bash_out_t::wrap (const std::string &src)
  {
    return "led-command.sh \"" + src + "\"";
  }

  void bash_out_t::stream_info (command_ptr_t command, const std::string &info)
  {
    auto status = std::make_shared<status_t>
      (command->id (), status_t::good (), info);
    m_status_queue.push (status);
  }

  void bash_out_t::clot_info (command_ptr_t command, const std::string &info)
  {
    command->result (info);
  }

  void bash_out_t::stream_error (command_ptr_t command)
  {
    log_t::buffer_t buf;
    buf << "bash: Error during stream command \"" << command->id ()
        << "\" handling";
    log_t::error (buf);
    erase (command);
  }

  void bash_out_t::clot_error (command_ptr_t command)
  {
    auto popen = command->popen ();
    auto status_value = (popen && popen->status ())
      ? (*(popen->status ())) : status_t::bad (); 
    auto status = std::make_shared<status_t>
      (command->id (), status_value, command->result ());

    m_status_queue.push (status);

    erase (command);
  }

  void bash_out_t::timeout (command_ptr_t command)
  {
    log_t::buffer_t buf;
    buf << "bash: Command \"" << command->id () << "\", execution timeout";
    log_t::error (buf);

    auto status = std::make_shared<status_t>
      (command->id (), status_t::timeout (), command->result ());

    m_status_queue.push (status);

    erase (command);
  }

  void bash_out_t::insert (command_ptr_t command)
  {
    std::lock_guard<std::mutex> guard (m_mutex);

    auto status = m_command_map.emplace
      (std::make_pair (command.get (), command));

    if (status.second == false) {
      log_t::buffer_t buf;
      buf << "bash: Failed to insert command \""
          << command->id () << "\"";
      log_t::error (buf);
    }
  }

  void bash_out_t::erase (command_ptr_t command)
  {
    auto erased_num = m_command_map.erase (command.get ());
    if (erased_num != 1) {
      log_t::buffer_t buf;
      buf << "bash: Failed to erase command \"" << command->id ()
          << "\"";
      log_t::error (buf);
    }
  }
} // led_d
