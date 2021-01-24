/*
 *
 */

#ifndef BASH_HPP
#define BASH_HPP

#include <atomic>
#include <map>
#include <mutex>

#include "asio.hpp"

#include "command-queue.hpp"
#include "status-queue.hpp"

namespace led_d
{

  class bash_t
  {
  public:
    bash_t (asio::io_context &io_context, status_queue_t &status_queue);
    ~bash_t () = default;

    void start ();
    void stop ();

    command_queue_t& command_queue () {return m_command_queue;}

  private:

    void execute_command (command_ptr_t command);

    std::string wrap (const std::string &src);

    void stream_info (command_ptr_t command, const std::string &info);
    void clot_info (command_ptr_t command, const std::string &info);

    void stream_error (command_ptr_t command);
    void clot_error (command_ptr_t command);

    void timeout (const asio::error_code &error, command_ptr_t command);

    void insert (command_ptr_t command);
    void erase (command_ptr_t command);

    asio::io_context &m_io_context;
    asio::steady_timer m_timeout_timer;

    status_queue_t &m_status_queue;

    command_queue_t m_command_queue;

    std::atomic_bool m_go;

    std::mutex m_mutex;
    std::map<command_t*, command_ptr_t> m_command_map;
  };

} // led_d

#endif

