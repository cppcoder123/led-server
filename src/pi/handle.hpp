/*
 *
 */
#ifndef HANDLE_HPP
#define HANDLE_HPP

#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <string>

#include "asio/asio.hpp"

#include "unix/condition-queue.hpp"

#include "command-queue.hpp"
#include "content.hpp"
#include "mcu-queue.hpp"
#include "mcu-msg.hpp"
#include "render.hpp"
#include "status.hpp"
#include "status-queue.hpp"

namespace led_d
{

  class handle_t
  {
  public:

    handle_t (asio::io_context &io_context, const arg_t &arg);
    handle_t (const handle_t&) = delete;
    ~handle_t () = default;

    void start ();
    void stop ();

    mcu_queue_t& from_mcu_queue () {return m_from_mcu_queue;}
    void to_mcu_queue (mcu_queue_t &queue);

    status_queue_t& status_queue () {return m_status_queue;}
    void command_queue (command_queue_t &queue) {m_command_queue = &queue;}

  private:
    using status_ptr_t = std::shared_ptr<status_t>;

    void notify ();

    void handle_status (status_ptr_t status);
    void handle_mcu (mcu_msg_t &msg);

    // handle mcu messages
    void mcu_rotor (const mcu_msg_t &msg);
    void mcu_status (const mcu_msg_t &msg);
    void mcu_version (const mcu_msg_t &msg);

    void info_push ();

    // void issue_command (command_id_t id,
    //                     std::string text, command_t::timeout_t timeout);

    bool filter_system (const std::string &info);

    std::mutex m_mutex;
    std::condition_variable m_condition;

    mcu_queue_t m_from_mcu_queue;
    mcu_queue_t *m_to_mcu_queue;

    command_queue_t *m_command_queue; // to bash
    status_queue_t m_status_queue;    // from bash

    content_t m_content;
    render_t m_render;

    std::atomic_bool m_go;
  };

} // namespace led_d

#endif
