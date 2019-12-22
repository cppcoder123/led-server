/*
 * Linux shell interaction
 */

#include <chrono>
#include <functional>

#include "unix/log.hpp"

#include "bash.hpp"
#include "popen.hpp"

namespace led_d
{
  constexpr auto system_command = "led-system.sh";
  constexpr auto track_name_command = "led-track-name.sh";
  constexpr auto clock_command = "led-clock.sh";

  bash_t::bash_t (asio::io_context &io_context,
                          status_queue_t &status_queue)
    : m_io_context (io_context),
      m_timeout_timer (m_io_context),
      m_status_queue (status_queue),
      m_go (true)
  {
  }

  void bash_t::start ()
  {
    auto command = std::make_shared<command_t>
      (STREAM_SYSTEM, system_command, command_t::infinity_timeout ());
    m_command_queue.push (command);

    command = std::make_shared<command_t>
      (STREAM_TRACK_NAME, track_name_command, command_t::infinity_timeout ());
    m_command_queue.push (command);

    command = std::make_shared<command_t>
      (STREAM_CLOCK, clock_command, command_t::infinity_timeout ());
    m_command_queue.push (command);

    while (m_go.load () == true) {
      auto command = m_command_queue.pop<true>();
      if (!command)
        continue;
      invoke_command (std::move(*command));
    }
  }

  void bash_t::stop ()
  {
    m_go.store (false);
    m_timeout_timer.cancel ();
    m_command_queue.notify_one<true> ();
  }

  void bash_t::invoke_command (command_ptr_t command)
  {
    auto body = (command->wrap ())
      ? wrap (command->body ()) : command->body ();

    auto info_cb = (command->stream ())
      ? std::bind
      (&bash_t::stream_info, this, command, std::placeholders::_1)
      : std::bind
      (&bash_t::clot_info, this, command, std::placeholders::_1);

    auto error_cb = (command->stream ())
      ? std::bind (&bash_t::stream_error, this, command)
      : std::bind (&bash_t::clot_error, this, command);

    auto timer = std::make_shared<command_t::timer_t>
      (m_io_context, std::chrono::steady_clock::now ());
    command->timer (timer);

    timer->expires_after (std::chrono::seconds (command->timeout ()));
    timer->async_wait
      (std::bind (&bash_t::timeout, this, std::placeholders::_1, command));

    // insert cmd before creating popen
    insert (command);

    auto popen = std::make_shared<popen_t>
      (body, m_io_context, info_cb, error_cb);
    command->popen (popen);
  }

  std::string bash_t::wrap (const std::string &src)
  {
    return "led-command.sh \"" + src + "\"";
  }

  void bash_t::stream_info (command_ptr_t command, const std::string &info)
  {
    auto status = std::make_shared<status_t>
      (command->id (), status_t::good (), info);
    m_status_queue.push (status);
  }

  void bash_t::clot_info (command_ptr_t command, const std::string &info)
  {
    command->result (info);
  }

  void bash_t::stream_error (command_ptr_t command)
  {
    command->cancel_timeout ();

    if (command->semi_stream () == false) {
      log_t::buffer_t buf;
      buf << "bash: Error during stream command \"" << command->id ()
          << "\" handling";
      log_t::error (buf);
      erase (command);
    }
  }

  void bash_t::clot_error (command_ptr_t command)
  {
    command->cancel_timeout ();

    auto popen = command->popen ();
    auto status_value = (popen && popen->status ())
      ? (*(popen->status ())) : status_t::bad (); 
    auto status = std::make_shared<status_t>
      (command->id (), status_value, command->result ());

    m_status_queue.push (status);

    erase (command);
  }

  void bash_t::timeout (const std::error_code &error, command_ptr_t command)
  {
    if (error)
      // timeout is cancelled
      return;

    log_t::buffer_t buf;
    buf << "bash: Command \"" << command->id () << "\", execution timeout";
    log_t::error (buf);

    auto status = std::make_shared<status_t>
      (command->id (), status_t::timeout (), command->result ());

    m_status_queue.push (status);

    erase (command);
  }

  void bash_t::insert (command_ptr_t command)
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

  void bash_t::erase (command_ptr_t command)
  {
    std::lock_guard<std::mutex> guard (m_mutex);

    auto erased_num = m_command_map.erase (command.get ());
    if (erased_num != 1) {
      log_t::buffer_t buf;
      buf << "bash: Failed to erase command \"" << command->id ()
          << "\"";
      log_t::error (buf);
    }
  }
} // led_d
