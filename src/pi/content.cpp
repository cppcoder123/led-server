//
//
//

#include "unix/format.hpp"
#include "unix/log.hpp"

#include "content.hpp"
#include "mcu-encode.hpp"
#include "mcu-id.hpp"
#include "popen.hpp"

namespace led_d
{
  constexpr auto MIN_INFO_SIZE = 3;

  const std::regex content_t::m_track_regex ("\\s*([^: ]+)\\s*:(.*)");
  const std::regex content_t::m_clock_regex ("\\s*(\\d+)\\s*:\\s*(\\d+).*");

  content_t::content_t (const std::list<std::string> &regex_list)
    : m_to_mcu_queue (nullptr),
      m_iterator (m_bottom_info.begin ())
  {
    for (auto &pattern_replace : regex_list) {
      // 1. split
      std::string pattern, replace;
      if (popen_t::split (pattern_replace, pattern, replace, m_track_regex) == false) {
        log_t::buffer_t buf;
        buf << "content: Failed to split regex_pattern \""
            << pattern_replace << "\"";
        log_t::error (buf);
        continue;
      }
      // 2. create regex
      auto regex = std::make_shared<std::regex> (pattern);
      // 3. put pair into list
      m_track_regex_list.push_back (std::make_pair (regex, replace));
    }
  }

  content_t::~content_t ()
  {
    // fixme
  }

  void content_t::in (status_ptr_t status)
  {
    switch (status->id ()) {
    case command_id_t::MENU_ADD:
      m_top_info += status->out ();
      break;
    case command_id_t::MENU_SET:
      // Note: it also sets empty value
      m_top_info = status->out ();
      break;
    case command_id_t::VOLUME_GET:
      // fixme: set current volume level
      break;
    case command_id_t::MPC_PLAY:
    case command_id_t::VOLUME_SET:
      // just ignore, status already checked
      break;
    case command_id_t::STREAM_SYSTEM:
      m_middle_info.push_back (status->out ());
      break;
    case command_id_t::STREAM_TRACK_NAME:
      if (status->out ().empty () == false)
        m_bottom_info[command_id_t::STREAM_TRACK_NAME] = replace (status->out ());
      break;
    case command_id_t::STREAM_CLOCK:
      {
        auto clock_info = status->out ();
        m_bottom_info[command_id_t::STREAM_CLOCK] = clock_info;
        sync_clock (clock_info);
      }
      break;
    default:
      log_t::buffer_t buf;
      buf << "content: Unknown status has arrived";
      log_t::error (buf);
      //m_bottom_info[status->id ()] = status->out ();
      break;
    }

    if (status->id () != command_id_t::STREAM_SYSTEM)
      m_iterator = m_bottom_info.find (status->id ());
  }

  content_t::out_info_t content_t::out ()
  {
    static const auto format = unix::format_t::encode_empty ();

    if (m_top_info.empty () == false)
      return std::make_pair (m_top_info, format);

    if (m_middle_info.empty () == false) {
      auto info = m_middle_info.front ();
      m_middle_info.pop_front ();
      return std::make_pair (info, format);
    }

    if (m_bottom_info.empty () == false) {
      if (m_iterator == m_bottom_info.end ())
        m_iterator = m_bottom_info.begin ();
      auto result = std::make_pair (m_iterator->second, format);
      ++m_iterator;
      return result;
    }

    return std::make_pair ("Content is empty, nothing to display ", format);
  }

  std::string content_t::replace (const std::string &src)
  {
    for (auto &re_replace : m_track_regex_list) {
      try {
        auto dst = std::regex_replace
          (src, *(re_replace.first), re_replace.second);
        if ((dst.empty () == false)
            && (dst != src))
          return dst;
      }
      catch (std::exception &e) {
        log_t::buffer_t buf;
        buf << "content: Failed to regex-replace into \""
            << re_replace.second << "\"";
        log_t::error (buf);
      }
    }

    return src;
  }

  void content_t::sync_clock (const std::string &time_src)
  {
    std::string hour_str, minute_str;
    if (popen_t::split (time_src,
                        hour_str, minute_str, m_clock_regex) == false) {
      log_t::buffer_t buf;
      buf << "content: Failed to split \"" << time_src
          << "\" into hours and minutes";
      log_t::error (buf);
      return;
    }

    std::istringstream stream (hour_str);
    unsigned hour = 0, minute = 0;
    stream >> hour;

    stream.clear ();
    stream.str (minute_str);
    stream >> minute;

#if 0
    // debug
    {
      log_t::buffer_t buf;
      buf << "content: clock-sync: hour: \"" << hour << "\", min : \""
          << minute << "\"";
      log_t::error (buf);
    }
    // debug
#endif

    auto msg = mcu::encode::join
      (mcu_id::get (), MSG_ID_SYNC_CLOCK, (uint8_t) hour, (uint8_t) minute);
    m_to_mcu_queue->push (msg);
  }

} // led_d
