/*
 *
 */
#ifndef HANDLE_HPP
#define HANDLE_HPP

#include <mutex>
#include <condition_variable>

#include "content.hpp"
#include "session.hpp"
#include "spi.hpp"
#include "type-def.hpp"

namespace led_d
{

  class handle_t
  {
  public:
    handle_t (unix_queue_t &unix_queue, mcu_queue_t &mcu_queue,
              content_t &content);
    handle_t (const handle_t&) = delete;
    ~handle_t () {};

    void start ();
    void stop ();

  private:

    void notify ();

    void handle_unix (unix_msg_t &msg);
    void handle_mcu (mcu_msg_t &msg);

    // handle messages
    void mcu_version (const mcu_msg_t &msg);
    void mcu_poll (const mcu_msg_t &msg);

    unix_queue_t &m_unix_queue;
    mcu_queue_t &m_mcu_queue;

    content_t &m_content;

    bool m_go;

    // special client, that consumes mcu messages like
    // button presses.
    // !!! Can be zero
    session_ptr_t m_client;

    std::mutex m_mutex;
    std::condition_variable m_condition;
  };

} // namespace led_d

#endif
