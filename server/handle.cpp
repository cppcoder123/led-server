/*
 *
 */

#include <functional>

// #include "unix/codec.hpp"
// #include "unix/refsymbol.hpp"
// #include "unix/request.hpp"
// #include "unix/response.hpp"

#include "handle.hpp"
#include "log-wrapper.hpp"
#include "matrix.hpp"
#include "mcu-decode.hpp"
#include "mcu-encode.hpp"
#include "serial-id.hpp"

namespace led_d
{
  handle_t::handle_t (const std::string &default_font, unix_queue_t &unix_queue,
                      mcu_queue_t &to_mcu_queue, mcu_queue_t &from_mcu_queue)
    : m_unix_queue (unix_queue),
      m_to_mcu_queue (to_mcu_queue),
      m_from_mcu_queue (from_mcu_queue),
      //m_content (content),
      m_render (default_font),
      m_go (true)
  {
    m_unix_queue.set_notify (std::bind (&handle_t::notify, this));
    m_from_mcu_queue.set_notify (std::bind (&handle_t::notify, this));
  }

  void handle_t::start ()
  {

    while (m_go == true) {
      auto unix_msg = m_unix_queue.pop ();
      auto mcu_msg = m_from_mcu_queue.pop ();
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
    request_t request;
    response_t response;

    std::string buffer;
    //
    if (request_codec_t::decode (msg.info, request) == false) {
      response.status = 1;
      response.string_data = "Failed to decode request message";
      log_t::error (response.string_data);
      if (response_codec_t::encode (response, buffer) == true)
        msg.sender->write (buffer);
      return;
    }

    response.status = 0;
    switch (request.action) {
    case request_t::subscribe:
      m_client = msg.sender;
      break;
    case request_t::insert:
      if (unix_insert (request) == false) {
        response.status = 1;
        response.string_data = "Failed to handle \"insert\" request";
        log_t::error (response.string_data);
      }
      break;
    default:
      {
        response.status = 1;
        response.string_data = "Unknown request has arrived";
        log_t::error (response.string_data);
      }
      break;
    }

    // Note: sender can be destroyed during async writing! (or not)
    // m_content.update (request, response);
    if (response_codec_t::encode (response, buffer) == true)
      msg.sender->write (buffer);
    else
      log_t::error ("Failed to encode \"response\" message");
  }

  void handle_t::handle_mcu (mcu_msg_t &msg)
  {
    char_t msg_id = mcu::decode::get_msg_id (msg);

    switch (msg_id) {
    case MSG_ID_VERSION:
      mcu_version (msg);
      break;
    case MSG_ID_POLL:
      mcu_poll ();
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

  void handle_t::mcu_version (const mcu_msg_t &msg)
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
    buf << "handle: Protocol version is confirmed!";
    log_t::info (buf);
  }

  void handle_t::mcu_poll ()
  {
    if (!m_client)
      return;

    response_t response;
    response.status = response_t::poll;

    std::string buf;
    if (response_codec_t::encode (response, buf) == true)
      m_client->write (buf);
    else {
      log_t::buffer_t buf;
      buf << "handle: Failed to encode \"poll\" response";
      log_t::error (buf);
    }
  }

  bool handle_t::unix_insert (const request_t &request)
  {
    matrix_t matrix;
    if (m_render.pixelize (matrix, request.info, request.format) == false) {
      log_t::buffer_t buf;
      buf << "Failed to pixelize info related to \"" << request.tag << "\"";
      log_t::error (buf);
      return false;
    }

    for (std::size_t i = 0; i < matrix.size (); ++i)
      m_to_mcu_queue.push
        (mcu::encode::join (serial::get (), MSG_ID_MONO_LED, matrix[i]));

    // {
    //   // fixme: debug
    //   log_t::buffer_t buf;
    //   buf << "request info: " << request.info;
    //   log_t::info (buf);
    // }

    return true;
  }

} // namespace led_d
