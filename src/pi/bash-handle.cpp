/*
 *
 */

#include <cstdint>
#include <functional>
#include <iterator>

#include "unix/log.hpp"

#include "bash-handle.hpp"
#include "matrix.hpp"
#include "mcu-decode.hpp"
#include "mcu-encode.hpp"
#include "mcu-id.hpp"

namespace led_d
{
  bash_handle_t::bash_handle_t (const std::string &default_font)
    : m_bash_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_from_mcu_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_to_mcu_queue (nullptr),
      m_render (default_font),
      m_go (true)
  {
  }

  void bash_handle_t::start ()
  {

    while (m_go.load () == true) {
      auto bash_msg = m_bash_queue.pop<false> ();
      if (bash_msg)
        handle_bash (*bash_msg);

      auto mcu_msg = m_from_mcu_queue.pop<false> ();
      if (mcu_msg)
        handle_mcu (*mcu_msg);

      {
        std::unique_lock lock (m_mutex);
        if ((m_bash_queue.empty<false> () == true)
            && (m_from_mcu_queue.empty<false> () == true))
          m_condition.wait (lock);
      }
    }
  }

  void bash_handle_t::stop ()
  {
    m_go.store (false);
    //
    notify ();
  }

  void bash_handle_t::notify ()
  {
    m_condition.notify_one ();
  }

  void bash_handle_t::handle_bash (std::string msg)
  {
    // fixme
  }
#if 0
  void bash_handle_t::handle_unix (bash_msg_t &msg)
  {
    request_t request;
    response_t response;

    std::string buffer;
    //
    if (codec_t::decode (msg.info, request) == false) {
      response.status = 1;
      response.string_data = "Failed to decode request message";
      log_t::error (response.string_data);
      if (codec_t::encode (response, buffer) == true)
        msg.sender->write (buffer);
      return;
    }

    response.status = 0;
    switch (request.action) {
    case request_t::subscribe:
      m_client = msg.sender;
      m_client->set_disconnect ([this](){m_client.reset ();});
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
    if (codec_t::encode (response, buffer) == true)
      msg.sender->write (buffer);
    else
      log_t::error ("Failed to encode \"response\" message");
  }
#endif

  void bash_handle_t::handle_mcu (mcu_msg_t &msg)
  {
    uint8_t msg_id = mcu::decode::get_msg_id (msg);

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
        buf << "bash-handle: Unknown message from mcu is arrived: "
            << (int) msg_id << " - ";
        for (auto i : msg) 
          buf << (int) i << " ";
        log_t::error (buf);
      }
      break;
    }
  }

  void bash_handle_t::mcu_version (const mcu_msg_t &msg)
  {
    uint8_t status = 0;
    if (mcu::decode::split_payload (msg, status) == false) {
      log_t::buffer_t buf;
      buf << "bash-handle: Failed to decode \"version\" message";
      log_t::error (buf);
      return;
    }

    if (status != STATUS_SUCCESS)
      throw std::runtime_error
        ("bash-handle: Pi & Mcu protocol version mismatch, can't continue...");

    log_t::buffer_t buf;
    buf << "bash-handle: Protocol version is confirmed!";
    log_t::info (buf);
  }

  void bash_handle_t::mcu_poll ()
  {
    // fixme
  }
#if 0
  void bash_handle_t::mcu_poll ()
  {
    if (!m_client)
      return;

    response_t response;
    response.status = response_t::poll;

    std::string buf;
    if (codec_t::encode (response, buf) == true)
      m_client->write (buf);
    else {
      log_t::buffer_t buf;
      buf << "bash-handle: Failed to encode \"poll\" response";
      log_t::error (buf);
    }
  }
#endif

  bool bash_handle_t::unix_insert (const request_t &request)
  {
    matrix_t matrix;
    if (m_render.pixelize (matrix, request.info, request.format) == false) {
      log_t::buffer_t buf;
      buf << "Failed to pixelize info related to \"" << request.tag << "\"";
      log_t::error (buf);
      return false;
    }

    std::size_t len = matrix.size () / LED_ARRAY_SIZE;
    auto start = matrix.begin ();
    
    for (std::size_t i = 0; i < len; ++i) {
      matrix_t::iterator finish = start;
      std::advance (finish, LED_ARRAY_SIZE);
      mcu_msg_t tmp (start, finish);
      m_to_mcu_queue->push
        (mcu::encode::join (mcu_id::get (), MSG_ID_LED_ARRAY, tmp));
      start = finish;
    }

    len = matrix.size () % LED_ARRAY_SIZE;
    for (std::size_t i = matrix.size ()- len; i < matrix.size (); ++i)
      m_to_mcu_queue->push
        (mcu::encode::join (mcu_id::get (), MSG_ID_LED, matrix[i]));
    
    // {
    //   // fixme: debug
    //   log_t::buffer_t buf;
    //   buf << "request info: " << request.info;
    //   log_t::info (buf);
    // }

    return true;
  }

} // namespace led_d
