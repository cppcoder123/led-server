/*
 *
 */

#include <functional>
#include <iterator>

#include "util/log.hpp"

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
  constexpr auto MPC_PLAY_PREFIX = "mpc play";
  constexpr auto MPC_PLAY = "led-mpc.sh play";
  constexpr auto MPC_PLAYLIST_GET = "led-mpc.sh playlist-get";
  constexpr auto MPC_PLAYLIST_SET = "led-mpc.sh playlist-set ";
  constexpr auto MPC_TRACK_GET = "led-mpc.sh track-get";
  constexpr auto MPC_TRACK_SET = "led-mpc.sh track-set ";
  constexpr auto MPC_VOLUME_GET = "led-mpc.sh volume-get";
  constexpr auto MPC_VOLUME_SET = "led-mpc.sh volume-set ";
  //
  constexpr auto POWEROFF = "sudo poweroff";
  constexpr auto REBOOT = "sudo reboot";
  //
  const std::regex system_regex ("\\s*([^:]+):(.*)");

  namespace {
    bool to_uint8 (const std::string &src, uint8_t &dst)
    {
      std::istringstream stream (src);
      unsigned tmp = 0;
      stream >> tmp;
      if ((stream.fail ()) || (tmp > 0xFF)) {
        log_t::buffer_t buf;
        buf << "handle: Failed to convert \"" << src
            << "\" to uint8_t";
        log_t::error (buf);
        return false;
      }

      dst = static_cast<uint8_t>(tmp);
      return true;
    }

    std::string from_uint8 (uint8_t src)
    {
      unsigned tmp = src;
      return std::to_string (tmp);
    }

    bool split_to_uint8 (const std::string &src, std::size_t &pos, uint8_t &dst)
    {
      constexpr auto info_delimiter = '-';
      auto new_pos = src.find (info_delimiter, pos);

      if (new_pos == std::string::npos) {
        auto tmp = src.substr (pos);
        pos = new_pos;
        return to_uint8 (tmp, dst);
      }

      auto tmp = src.substr (pos, new_pos - pos);
      pos = new_pos + 1;
      return to_uint8 (tmp, dst);
    }

    bool split_info (const std::string &src,
                     uint8_t &value, uint8_t &min, uint8_t &max)
    {
      std::size_t pos = 0;
      return ((split_to_uint8 (src, pos, value) == false)
              || (split_to_uint8 (src, pos, min) == false)
              || (split_to_uint8 (src, pos, max) == false)) ? false : true;
    }
  }

  handle_t::handle_t (asio::io_context &io_context, const arg_t &arg)
    : m_from_mcu_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_to_mcu_queue (nullptr),
      m_command_queue (nullptr),
      m_status_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_content (arg.subject_regexp_list),
      m_suspend (false),
      m_go (true),
      m_playlist_id (0)
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
    // m_menu.command_queue (queue);
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
          << "\" has arrived for command \"" << command_id_name (status->id ())
          << "\"";
      log_t::error (buf);
      return;
    }

    if ((status->id () == command_id_t::STREAM_SYSTEM)
        && (filter_system (status->out ()) == true))
      return;

    switch (status->id ()) {
    case command_id_t::MPC_PLAYLIST_GET:
    case command_id_t::MPC_TRACK_GET:
    case command_id_t::MPC_VOLUME_GET:
      {
        uint8_t value = 0, min = 0, max = 0;
        if (split_info (status->out (), value, min, max) == false) {
          log_t::buffer_t buf;
          buf << "handle:: Failed to split \"" << status->out ()
              << "\" into value-min-max";
          log_t::error (buf);
          return;
        }
        uint8_t param =
          (status->id () == command_id_t::MPC_VOLUME_GET) ? PARAMETER_VOLUME
          : (status->id () == command_id_t::MPC_TRACK_GET) ? PARAMETER_TRACK
          : PARAMETER_PLAYLIST;
        if (param == PARAMETER_PLAYLIST)
          // mpd can't recall last loaded playlist
          value = m_playlist_id;
        m_to_mcu_queue->push
          (mcu::encode::join
           (mcu_id::get (), MSG_ID_PARAM_QUERY, param, value, min, max));
      }
      break;
    case command_id_t::MPC_PLAYLIST_SET:
    case command_id_t::MPC_TRACK_SET:
    case command_id_t::MPC_VOLUME_SET:
      // ignore
      break;
    default:
      if (status->value () == status_t::good ())
        m_content.in (status);
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
    case MSG_ID_PARAM_QUERY:
      mcu_param_query (msg);
      break;
    case MSG_ID_PARAM_SET:
      mcu_param_set (msg);
      break;
    case MSG_ID_POWEROFF:
      mcu_poweroff ();
      break;
    case MSG_ID_REBOOT:
      mcu_reboot ();
      break;
    case MSG_ID_RESUME:
      mcu_resume ();
      break;
    case MSG_ID_STATUS:
      mcu_status (msg);
      break;
    case MSG_ID_SUSPEND:
      mcu_suspend ();
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

  void handle_t::mcu_param_query (const mcu_msg_t &msg)
  {
    uint8_t param = 0;
    if (mcu::decode::split_payload (msg, param) == false) {
      log_t::error ("handle: Failed to decode param-query message");
      return;
    }
    if ((param != PARAMETER_VOLUME)
        && (param != PARAMETER_TRACK)
        && (param != PARAMETER_PLAYLIST)) {
      log_t::error ("handle: Bad parameter in param-query message");
      return;
    }

    command_id_t id = command_id_t::MPC_PLAYLIST_GET;
    std::string cmd = MPC_PLAYLIST_GET;

    if (param == PARAMETER_PLAYLIST) {
      id = command_id_t::MPC_PLAYLIST_GET;
      cmd = MPC_PLAYLIST_GET;
    } else if (param == PARAMETER_TRACK) {
      id = command_id_t::MPC_TRACK_GET;
      cmd = MPC_TRACK_GET;
    } else {
      id = command_id_t::MPC_VOLUME_GET;
      cmd = MPC_VOLUME_GET;
    }

    command_t::issue (id, cmd, command_t::three_seconds (), m_command_queue);
  }

  void handle_t::mcu_param_set (const mcu_msg_t &msg)
  {
    uint8_t param, positive, delta;
    if (mcu::decode::split_payload (msg, param, positive, delta) == false) {
      log_t::error ("handle: Failed to decode param-set message");
      return;
    }

    if (delta == 0)
      // is it an error? do we need to report?
      return;

    if (((param != PARAMETER_VOLUME)
         && (param != PARAMETER_TRACK)
         && (param != PARAMETER_PLAYLIST))
        || ((positive != PARAMETER_POSITIVE)
            && (positive != PARAMETER_NEGATIVE))) {
      log_t::error ("handle: Bad value(s) in param-set message");
      return;
    }

    std::string cmd = (param == PARAMETER_VOLUME) ? MPC_VOLUME_SET
      : (param == PARAMETER_TRACK) ? MPC_TRACK_SET
      : MPC_PLAYLIST_SET;
    if (param != PARAMETER_PLAYLIST) {
      cmd += (positive == PARAMETER_POSITIVE) ? "+ " : "- ";
      cmd += from_uint8 (delta);
    } else {
      const uint8_t new_id = (positive == PARAMETER_POSITIVE)
        ? m_playlist_id + delta
        : (m_playlist_id > delta) ? m_playlist_id - delta
        : 0;
      cmd += from_uint8 (new_id);
      if (new_id > 0)
        m_playlist_id = new_id;
    }

    command_id_t cmd_id
      = (param == PARAMETER_VOLUME) ? command_id_t::MPC_VOLUME_SET
      : (param == PARAMETER_TRACK) ? command_id_t::MPC_TRACK_SET
      : command_id_t::MPC_PLAYLIST_SET;

    command_t::issue (cmd_id, cmd,
                      command_t::three_seconds (), m_command_queue);
  }

  void handle_t::mcu_poweroff ()
  {
    command_t::issue (command_id_t::POWEROFF, POWEROFF,
                      command_t::ten_seconds (), m_command_queue);
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

  void handle_t::mcu_suspend ()
  {
    m_suspend = true;
  }

  void handle_t::mcu_reboot ()
  {
    command_t::issue (command_id_t::REBOOT, REBOOT,
                      command_t::ten_seconds (), m_command_queue);
  }

  void handle_t::mcu_resume ()
  {
    m_suspend = false;
    info_push ();
  }

  void handle_t::mcu_version (const mcu_msg_t &msg)
  {
    uint8_t version = 0;
    if (mcu::decode::split_payload (msg, version) == false) {
      log_t::buffer_t buf;
      buf << "handle: Failed to decode \"version\" message";
      log_t::error (buf);
      return;
    }

    if (version != PROTOCOL_VERSION)
      throw std::runtime_error
        ("handle: Pi & Mcu protocol version mismatch, can't continue...");

    log_t::buffer_t buf;
    buf << "handle: Protocol version is OK!";
    log_t::info (buf);

    m_to_mcu_queue->push
      (mcu::encode::join (SERIAL_ID_TO_IGNORE, MSG_ID_HANDSHAKE));

    info_push ();
  }

  void handle_t::info_push ()
  {
    if ((m_suspend == true)
        || (m_to_mcu_queue->size<true>() >= QUEUE_SIZE_LIMIT))
      return;

    auto content_info = m_content.out ();
    auto info = content_info.text + " ";
    auto &format = content_info.format;

    // {
    //   log_t::buffer_t buf;
    //   buf << "info: " << info;
    //   log_t::info (buf);
    // }
    
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
      std::advance (finish, len);
      mcu_msg_t tmp (start, finish);
      m_to_mcu_queue->push
        (mcu::encode::join
         (mcu_id::get (), MSG_ID_LED_ARRAY, tmp));
    }

    // {
    //   // debug
    //   log_t::buffer_t buf;
    //   buf << "handle: To-mcu-queue size is: " << m_to_mcu_queue->size<true> ();
    //   log_t::info (buf);
    // }
  }

  bool handle_t::filter_system (const std::string &info)
  {
    std::string prefix, suffix;
    if (popen_t::split (info, prefix, suffix, system_regex) == false)
      return false;

    if (prefix == MPC_PLAY_PREFIX) {
      command_t::issue
        (command_id_t::MPC_PLAY, MPC_PLAY,
         command_t::three_seconds (), m_command_queue);
      return true;
    }

    return false;
  }

} // namespace led_d
