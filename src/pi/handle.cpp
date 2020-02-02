/*
 *
 */

#include <cstdint>
#include <functional>
#include <iterator>

#include "unix/log.hpp"

#include "command-issue.hpp"
#include "handle.hpp"
#include "matrix.hpp"
#include "mcu-decode.hpp"
#include "mcu-encode.hpp"
#include "mcu-id.hpp"
#include "popen.hpp"

namespace led_d
{
  // we should provide info if size is less
  constexpr auto QUEUE_SIZE_LIMIT = 3;
  //
  constexpr auto MPC_PLAY = "mpc play";
  constexpr auto MPC_PLAYLIST = "mpc playlist";
  constexpr auto VOLUME_RANGE = "volume range";
  //
  constexpr auto MATRIX_MIN_SIZE = 64;
  //
  const std::regex system_regex ("\\s*([^:]+):(.*)");
  const std::regex volume_regex ("\\s*(\\d+)-(\\d+)\\s*");

  handle_t::handle_t (asio::io_context &io_context, const arg_t &arg)
    : m_from_mcu_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_to_mcu_queue (nullptr),
      m_command_queue (nullptr),
      m_status_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_content (arg.subject_regexp_list),
      m_menu (io_context, m_status_queue),
      m_render (arg.default_font),
      m_go (true)
  {
  }

  void handle_t::start ()
  {

    while (m_go.load () == true) {
      auto status = m_status_queue.pop<false> ();
      if (status)
        handle_status (*status);
        //m_content.in (*status);

      auto mcu_msg = m_from_mcu_queue.pop<false> ();
      if (mcu_msg)
        handle_mcu (*mcu_msg);

      {
        std::unique_lock lock (m_mutex);
        if ((m_status_queue.empty<false> () == true)
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

  void handle_t::to_mcu_queue (mcu_queue_t &queue)
  {
    m_to_mcu_queue = &queue;
    m_content.to_mcu_queue (queue);
  }

  void handle_t::command_queue (command_queue_t &queue)
  {
    m_command_queue = &queue;
    m_menu.command_queue (queue);
  }

  void handle_t::notify ()
  {
    m_condition.notify_one ();
  }

  void handle_t::handle_status (status_ptr_t status)
  {
    if (status->value () != status_t::good ()) {
      log_t::buffer_t buf;
      buf << "handle: Bad status \"" << status->value ()
          << "\" has arrived for command \"" << command_id_name (status->id ());
      log_t::error (buf);
      return;
    }

    if ((status->id () == command_id_t::STREAM_SYSTEM)
        && (filter_system (status->out ()) == true))
      return;

    switch (status->id ()) {
    case command_id_t::MPC_PLAYLIST:
      {
        auto text = status->out ();
        m_menu.track_add (text);
        
        if ((text.empty () == true)
            && (status->value () != status_t::good ()))
          // don't keep error text
          m_menu.track_clear ();

        // fixme: remove
        log_t::buffer_t buf;
        buf << "play-list: \"" << status->out () << "\"";
        log_t::info (buf);
        
      }
      break;
    case command_id_t::MPC_CURRENT:
      m_menu.current_track (status->out ());
      break;
    case command_id_t::VOLUME_GET:
      m_menu.current_volume (status->out ());
      break;
    case command_id_t::VOLUME_SET:
      // ignore
      break;
    default:
      if (status->value () == status_t::good ()) {
        m_content.in (status);
      } else {
        log_t::buffer_t buf;
        buf << "Bad status \"" << status->value () << "\" arrived for command \""
            << static_cast<int>(status->id ()) << "\"";
        log_t::error (buf);
      }
      break;
    }
  }

  void handle_t::handle_mcu (mcu_msg_t &msg)
  {
    uint8_t msg_id = 0;
    if (mcu::decode::get_msg_id (msg, msg_id) == false) {
      log_t::error ("handle: Failed to parse mcu msg");
      return;
    }

    switch (msg_id) {
    case MSG_ID_POLL:
      info_push ();
      break;
    case MSG_ID_ROTOR:
      mcu_rotor (msg);
      break;
    case MSG_ID_STATUS:
      mcu_status (msg);
      break;
    case MSG_ID_VERSION:
      mcu_version (msg);
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

  void handle_t::mcu_rotor (const mcu_msg_t &msg)
  {
    uint8_t id = 0;
    uint8_t action = 0;
    if (mcu::decode::split_payload (msg, id, action) == false) {
      log_t::error ("handle: Failed to decode rotor message");
      return;
    }

    m_menu.rotor (id, action);
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

  void handle_t::info_push ()
  {
    if (m_to_mcu_queue->size<true>() >= QUEUE_SIZE_LIMIT)
      return;

    auto content_info = m_content.out ();
    auto info = content_info.text + " ";
    auto &format = content_info.format;
    bool clear_buffer = (content_info.flag) ? true : false;
    if (clear_buffer == true) {
      m_to_mcu_queue->clear ();
      m_to_mcu_queue->push (mcu::encode::join (mcu_id::get (), MSG_ID_CLEAR));
    }

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

    if (clear_buffer == true) {
        if (matrix.size () < MATRIX_MIN_SIZE)
          // add zeros to make info visible
          matrix.insert (matrix.end (), MATRIX_MIN_SIZE - matrix.size (), 0);
        else if (matrix.size () > MATRIX_MIN_SIZE) {
          auto iter = matrix.begin ();
          std::advance (iter, MATRIX_MIN_SIZE);
          matrix.erase (iter, matrix.end ());
        }
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
      std::advance (finish, len);
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

  // void handle_t::issue_command (command_id_t id,
  //                               std::string text, command_t::timeout_t timeout)
  // {
  //   auto command = std::make_shared<command_t>(id, text, timeout);
  //   m_command_queue->push (command);
  // }

  bool handle_t::filter_system (const std::string &info)
  {
    std::string prefix, suffix;
    if (popen_t::split (info, prefix, suffix, system_regex) == false)
      return false;

    if (prefix == MPC_PLAY) {
      command_issue
        (command_id_t::MPC_PLAY, MPC_PLAY,
         command_t::three_seconds (), *m_command_queue);
      return true;
    } else if (prefix == MPC_PLAYLIST) {
      command_issue
        (command_id_t::MPC_PLAYLIST, MPC_PLAYLIST,
         command_t::three_seconds (), *m_command_queue);
      return true;
    } else if (prefix == VOLUME_RANGE) {
      std::string low, high;
      if (popen_t::split (suffix, low, high, volume_regex) == false) {
        log_t::buffer_t buf;
        buf << "handle: Failed to parse volume range";
        log_t::error (buf);
        return true;
      }
      m_menu.volume_range (low, high);
    }

    return false;
  }

} // namespace led_d
