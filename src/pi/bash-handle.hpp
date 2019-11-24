/*
 *
 */
#ifndef BASH_HANDLE_HPP
#define BASH_HANDLE_HPP

#include <condition_variable>
#include <list>
#include <mutex>

#include "unix/codec.hpp"
#include "unix/condition-queue.hpp"
#include "unix/refsymbol.hpp"
#include "unix/request.hpp"
#include "unix/response.hpp"

#include "bash-queue.hpp"
#include "mcu-queue.hpp"
#include "mcu-msg.hpp"
#include "render.hpp"

namespace led_d
{

  class bash_handle_t
  {
  public:

    bash_handle_t (const std::string &default_font);
    bash_handle_t (const bash_handle_t&) = delete;
    ~bash_handle_t () {};

    void start ();
    void stop ();

    bash_queue_t& bash_queue () {return m_bash_queue;}
    mcu_queue_t& from_mcu_queue () {return m_from_mcu_queue;}
    void to_mcu_queue (mcu_queue_t &to_mcu_queue) {m_to_mcu_queue = &to_mcu_queue;}

  private:

    using refsymbol_t = unix::refsymbol_t;
    using request_t = unix::request_t;
    using response_t = unix::response_t;
    using codec_t = unix::codec_t<refsymbol_t>;

    void notify ();

    void handle_unix (bash_msg_t &msg);
    void handle_mcu (mcu_msg_t &msg);

    // handle mcu messages
    void mcu_version (const mcu_msg_t &msg);
    void mcu_poll ();

    // handle unix messages
    bool unix_insert (const request_t &request);

    std::mutex m_mutex;
    std::condition_variable m_condition;
    bash_queue_t m_bash_queue;

    mcu_queue_t m_from_mcu_queue;
    mcu_queue_t *m_to_mcu_queue;

    //content_t &m_content;
    render_t m_render;

    bool m_go;

    // special client, that consumes mcu messages like
    // button presses.
    // !!! Can be zero
    // session_ptr_t m_client;
  };

} // namespace led_d

#endif
