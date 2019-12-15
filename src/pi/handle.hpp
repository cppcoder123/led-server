/*
 *
 */
#ifndef HANDLE_HPP
#define HANDLE_HPP

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <string>

#include "unix/condition-queue.hpp"

#include "bash-queue.hpp"
#include "content.hpp"
#include "mcu-queue.hpp"
#include "mcu-msg.hpp"
#include "render.hpp"

namespace led_d
{

  class handle_t
  {
  public:

    handle_t (const std::string &default_font,
              const std::list<std::string> &regexp_list);
    handle_t (const handle_t&) = delete;
    ~handle_t () {};

    void start ();
    void stop ();

    bash_queue_t& bash_queue () {return m_bash_queue;}
    mcu_queue_t& from_mcu_queue () {return m_from_mcu_queue;}
    void to_mcu_queue (mcu_queue_t &to_mcu_queue) {m_to_mcu_queue = &to_mcu_queue;}

  private:

    void notify ();

    void handle_mcu (mcu_msg_t &msg);

    // handle mcu messages
    void mcu_version (const mcu_msg_t &msg);
    void mcu_status (const mcu_msg_t &msg);

    void info_push ();

    std::mutex m_mutex;
    std::condition_variable m_condition;
    bash_queue_t m_bash_queue;

    mcu_queue_t m_from_mcu_queue;
    mcu_queue_t *m_to_mcu_queue;

    content_t m_content;
    render_t m_render;

    std::atomic_bool m_go;
  };

} // namespace led_d

#endif
