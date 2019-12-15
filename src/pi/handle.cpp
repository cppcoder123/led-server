/*
 *
 */

#include <cstdint>
#include <functional>
#include <iterator>

#include "unix/log.hpp"

#include "handle.hpp"
#include "matrix.hpp"
#include "mcu-decode.hpp"
#include "mcu-encode.hpp"
#include "mcu-id.hpp"

namespace led_d
{
  // we should provide info if size is less
  constexpr auto QUEUE_SIZE_LIMIT = 3;

  handle_t::handle_t (const std::string &default_font,
                      const std::list<std::string> &regexp_list)
    : m_bash_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_from_mcu_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_to_mcu_queue (nullptr),
      m_content (regexp_list),
      m_render (default_font),
      m_go (true)
  {
  }

  void handle_t::start ()
  {

    while (m_go.load () == true) {
      auto bash_msg = m_bash_queue.pop<false> ();
      if (bash_msg)
        m_content.in (*bash_msg);

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

  void handle_t::stop ()
  {
    m_go.store (false);
    //
    notify ();
  }

  void handle_t::notify ()
  {
    m_condition.notify_one ();
  }

  void handle_t::handle_mcu (mcu_msg_t &msg)
  {
    uint8_t msg_id = 0;
    if (mcu::decode::get_msg_id (msg, msg_id) == false) {
      log_t::error ("handle: Failed to parse mcu msg");
      return;
    }

    switch (msg_id) {
    case MSG_ID_VERSION:
      mcu_version (msg);
      break;
    case MSG_ID_STATUS:
      mcu_status (msg);
      break;
    // case MSG_ID_POLL:
    //   {
    //     uint8_t buf_space = 0;
    //     if (mcu::decode::split_payload (msg, buf_space) == false) {
    //       log_t::error ("handle: Failed to parse buffer size");
    //       return;
    //     }
    //     if (m_to_mcu_queue->size<true>() >= PI_QUEUE_LIMIT)
    //       return;

    //     {
    //       log_t::buffer_t buf;
    //       buf << "handle: poll space: " << (int) buf_space;
    //       log_t::info (buf);
    //     }

    //     auto info = m_content.out ();
    //     info_push (info.first, info.second);
    //   }
    //   break;
    default:
      {
        log_t::buffer_t buf;
        buf << "handle: Unknown message from mcu is arrived: "
            << (int) msg_id << " - ";
        for (auto i : msg) 
          buf << (int) i << " ";
        log_t::error (buf);
      }
      break;
    }
  }

  void handle_t::mcu_version (const mcu_msg_t &msg)
  {
    uint8_t status = 0;
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

    info_push ();
  }

  void handle_t::mcu_status (const mcu_msg_t &msg)
  {
    uint8_t status = 0;
    uint8_t src_msg_id = MSG_ID_EMPTY;
    if (mcu::decode::split_payload (msg, status, src_msg_id) == false) {
      log_t::error ("handle: Failed to decode status message");
      return;
    }
    if (status != STATUS_SUCCESS) {
      log_t::buffer_t buf;
      buf << "handle: Bad status \"" << (int) status
          << "\" has arrived for src-msg-id \"" << (int) src_msg_id << "\"";
      log_t::error (buf);
    }
    if (src_msg_id == MSG_ID_LED_ARRAY)
      info_push ();
  }

  void handle_t::info_push ()
  {
    if (m_to_mcu_queue->size<true>() >= QUEUE_SIZE_LIMIT)
      return;

    auto content_info = m_content.out ();
    auto &info = content_info.first;
    auto &format = content_info.second;

    {
      log_t::buffer_t buf;
      buf << "info: " << info;
      log_t::info (buf);
    }
    
    matrix_t matrix;
    if (m_render.pixelize (matrix, info, format) == false) {
      log_t::buffer_t buf;
      buf << "Failed to pixelize info \"" << info << "\"";
      log_t::error (buf);
      return;
    }

    std::size_t len = matrix.size () / LED_ARRAY_SIZE;
    auto start = matrix.begin ();
    matrix_t::iterator finish = start;
    
    for (std::size_t i = 0; i < len; ++i) {
      std::advance (finish, LED_ARRAY_SIZE);
      mcu_msg_t tmp (start, finish);
      m_to_mcu_queue->push
        (mcu::encode::join
         (mcu_id::get (), MSG_ID_LED_ARRAY, tmp));
      start = finish;
    }

    len = matrix.size () % LED_ARRAY_SIZE;
    if (len > 0) {
      std::advance (finish, matrix.size () % LED_ARRAY_SIZE);
      mcu_msg_t tmp (start, finish);
      m_to_mcu_queue->push
        (mcu::encode::join
         (mcu_id::get (), MSG_ID_LED_ARRAY, tmp));
    }

    {
      // debug
      log_t::buffer_t buf;
      buf << "handle: To-mcu-queue size is: " << m_to_mcu_queue->size<true> ();
      log_t::info (buf);
    }
  }

} // namespace led_d
