/*
 *
 */

#include <functional>

#include "unix/codec.hpp"
#include "unix/refsymbol.hpp"
#include "unix/request.hpp"
#include "unix/response.hpp"

#include "handle.hpp"
#include "log-wrapper.hpp"
#include "mcu-decode.hpp"

namespace led_d
{
  namespace
  {
    void handle_version (const mcu_msg_t &msg)
    {
      char_t status = 0;
      if (mcu::decode::split_payload (msg, status) == false) {
        log_t::buffer_t buf;
        buf << "handle: Failed to decode \"version\" message";
        log_t::error (buf);
        return;
      }

      if (status != STATUS_SUCCESS)
        throw std::runtime_error
          ("handle: Pi & Mcu protocol version mismatch, can't continue...");

      log_t::buffer_t buf;
      buf << "handle: Protocol \"version\" is correct!";
      log_t::info (buf);
    }
  } // namespace

  handle_t::handle_t (unix_queue_t &unix_queue,
                      mcu_queue_t &mcu_queue, content_t &content)
    : m_unix_queue (unix_queue),
      m_mcu_queue (mcu_queue),
      m_content (content),
      m_go (true)
  {
    m_unix_queue.set_notify (std::bind (&handle_t::notify, this));
    m_mcu_queue.set_notify (std::bind (&handle_t::notify, this));
  }

  void handle_t::start ()
  {

    while (m_go == true) {
      auto unix_msg = m_unix_queue.pop ();
      auto mcu_msg = m_mcu_queue.pop ();
      if ((!unix_msg)
          && (!mcu_msg)) {
        std::unique_lock<std::mutex> lock (m_mutex);
        m_condition.wait (lock);
        continue;
      }
      if (unix_msg)
        handle_unix (**unix_msg);
      if (mcu_msg)
        handle_mcu (*mcu_msg);
    }
  }

  void handle_t::stop ()
  {
    m_go = false;
    //
    notify ();
  }

  void handle_t::notify ()
  {
    m_condition.notify_one ();
  }

  void handle_t::handle_unix (unix_msg_t &msg)
  {
    using refsymbol_t = unix::refsymbol_t;
    using request_t = unix::request_t;
    using response_t = unix::response_t;
    using request_codec_t = unix::codec_t<refsymbol_t, request_t>;
    using response_codec_t = unix::codec_t<refsymbol_t, response_t>;
    //

    request_t request;
    response_t response;

    std::string buffer;
    //
    if (request_codec_t::decode (msg.info, request) == false) {
      response.status = 1;
      response.reason = "Failed to decode request message";
      log_t::error (response.reason);
      if (response_codec_t::encode (response, buffer) == true)
        msg.sender->send (buffer);
      return;
    }

    m_content.update (request, response);
    if (response_codec_t::encode (response, buffer) == true)
      msg.sender->send (buffer);
  }

  void handle_t::handle_mcu (mcu_msg_t &msg)
  {
    char_t msg_id = mcu::decode::get_msg_id (msg);

    switch (msg_id) {
    case MSG_ID_VERSION:
      handle_version (msg);
      break;
    default:
      {
        log_t::buffer_t buf;
        buf << "handle: Unknown message from mcu is arrived";
        log_t::error (buf);
      }
      break;
    }
  }
} // namespace led_d
