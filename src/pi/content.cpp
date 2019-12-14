//
//
//

#include "unix/format.hpp"
#include "unix/log.hpp"

#include "content.hpp"
#include "popen.hpp"

namespace led_d
{
  constexpr auto MIN_INFO_SIZE = 3;
  constexpr auto sys = "sys";
  constexpr auto mpd = "mpd";

  const std::regex content_t::m_regex ("\\s*([^: ]+)\\s*:(.*)");

  content_t::content_t (const std::list<std::string> &regex_list)
    : m_iterator (m_info.begin ())
  {
    for (auto &pattern_replace : regex_list) {
      // 1. split
      std::string pattern, replace;
      if (popen_t::split (pattern_replace, pattern, replace, m_regex) == false) {
        log_t::buffer_t buf;
        buf << "content: Failed to split regex_pattern \""
            << pattern_replace << "\"";
        log_t::error (buf);
        continue;
      }
      // 2. create regex
      auto regex = std::make_shared<std::regex> (pattern);
      // 3. put pair into list
      m_regex_list.push_back (std::make_pair (regex, replace));
    }
  }

  content_t::~content_t ()
  {
    // fixme
  }

  void content_t::in (std::string info)
  {
    std::string prefix, suffix;
    if (popen_t::split (info, prefix, suffix, m_regex) == false) {
      log_t::buffer_t buf;
      buf << "content: Failed to split info \"" << info << "\"";
      log_t::error (buf);
      return;
    }

    if (prefix == sys) {
      m_sys_info.push_back (suffix);
      return;
    }

    m_info[prefix] = (prefix == mpd) ? replace (suffix) : suffix;
    m_iterator = m_info.find (prefix);
  }

  content_t::out_info_t content_t::out ()
  {
    static const auto format = unix::format_t::encode_empty ();

    if (m_sys_info.empty () == false) {
      auto info = m_sys_info.front ();
      m_sys_info.pop_front ();
      return std::make_pair (info, format);
    }

    if (m_info.empty () == false) {
      if (m_iterator == m_info.end ())
        m_iterator = m_info.begin ();
      auto result = std::make_pair (m_iterator->second, format);
      ++m_iterator;
      return result;
    }

    //log_t::buffer_t buf;
    //log_t::error ("Empty info list");
    return std::make_pair ("Content is empty, nothing to display ", format);
  }

  std::string content_t::replace (const std::string &src)
  {
    for (auto &re_replace : m_regex_list) {
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
} // led_d
