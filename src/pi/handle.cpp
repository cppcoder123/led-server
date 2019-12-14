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
  constexpr auto MCU_BUFFER_LIMIT = 128;

  handle_t::handle_t (const std::string &default_font)
    : m_bash_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_from_mcu_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_to_mcu_queue (nullptr),
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
    uint8_t msg_id = mcu::decode::get_msg_id (msg);

    switch (msg_id) {
    case MSG_ID_VERSION:
      mcu_version (msg);
      break;
    case MSG_ID_POLL:
      {
        uint8_t buf_space = 0;
        if (mcu::decode::split_payload (msg, buf_space) == false) {
          log_t::error ("handle: Failed to parse buffer size");
          return;
        }
        if (buf_space <= MCU_BUFFER_LIMIT)
          return;

        auto info = m_content.out ();
        info_push (info.first, info.second);
      }
      break;
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
  }

  bool handle_t::info_push (std::string info, std::string format)
  {
    {
      log_t::buffer_t buf;
      buf << "request info: " << info;
      log_t::info (buf);
    }
    
    matrix_t matrix;
    if (m_render.pixelize (matrix, info, format) == false) {
      log_t::buffer_t buf;
      buf << "Failed to pixelize info \"" << info << "\"";
      log_t::error (buf);
      return false;
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
        
    return true;
  }

} // namespace led_d
