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

  const std::regex content_t::m_regex ("\\s*([^: ]+)\\s*:(.*)");

  content_t::content_t ()
  {
    // fixme
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
      m_info_list.push_back (info);
      return;
    }

    m_info_list.push_back (suffix);
  }

  content_t::out_info_t content_t::out ()
  {
    static const auto format = unix::format_t::encode_empty ();

    if (m_info_list.empty () == true) {
      log_t::buffer_t buf;
      log_t::error ("Empty info list");
      return std::make_pair ("No-info ", format);
    }

    auto info = m_info_list.front ();
    m_info_list.pop_front ();

    if (m_info_list.size () < MIN_INFO_SIZE)
      // fixme: call bash_in::kick
      ;

    return std::make_pair (info, format);
  }
} // led_d
