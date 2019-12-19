/*
 *
 */

#ifndef BASH_OUT_HPP
#define BASH_OUT_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "asio/asio.hpp"

#include "command-queue.hpp"
#include "status-queue.hpp"

namespace led_d
{

  class bash_out_t
  {
  public:
    bash_out_t (asio::io_context &io_context, status_queue_t &status_queue);
    ~bash_out_t () = default;

    void start ();
    void stop ();

    command_queue_t& command_queue () {return m_command_queue;}

  private:

    void invoke_command (command_ptr_t command);

    std::string wrap (const std::string &src);

    void stream_info (const std::string &info);
    void clot_info (const std::string &info);

    void stream_error (command_ptr_t command);
    void clot_error (command_ptr_t command);

    void insert (command_ptr_t command);

    asio::io_context &m_io_context;
    status_queue_t &m_status_queue;

    command_queue_t m_command_queue;

    std::atomic_bool m_go;

  };

} // led_d

#endif

