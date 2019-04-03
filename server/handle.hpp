/*
 *
 */
#ifndef HANDLE_HPP
#define HANDLE_HPP

#include <mutex>
#include <condition_variable>

#include "unix/codec.hpp"
#include "unix/refsymbol.hpp"
#include "unix/request.hpp"
#include "unix/response.hpp"

#include "render.hpp"
#include "session.hpp"
#include "spi.hpp"
#include "type-def.hpp"

namespace led_d
{

  class handle_t
  {
  public:
    handle_t (const std::string &default_font, unix_queue_t &unix_queue,
              mcu_queue_t &to_mcu_queue, mcu_queue_t &from_mcu_queue);
    handle_t (const handle_t&) = delete;
    ~handle_t () {};

    void start ();
    void stop ();

  private:

    using refsymbol_t = unix::refsymbol_t;
    using request_t = unix::request_t;
    using response_t = unix::response_t;
    using codec_t = unix::codec_t<refsymbol_t>;

    void notify ();

    void handle_unix (unix_msg_t &msg);
    void handle_mcu (mcu_msg_t &msg);

    // handle mcu messages
    void mcu_version (const mcu_msg_t &msg);
    void mcu_poll ();

    // handle unix messages
    bool unix_insert (const request_t &request);

    unix_queue_t &m_unix_queue;

    mcu_queue_t &m_to_mcu_queue;
    mcu_queue_t &m_from_mcu_queue;

    //content_t &m_content;
    render_t m_render;

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
