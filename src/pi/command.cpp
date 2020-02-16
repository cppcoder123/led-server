/*
 *
 */

#include "command.hpp"

namespace led_d
{
  constexpr auto result_delimiter = "\n";

  command_t::command_t (command_id_t id, std::string body, timeout_t time_out)
    : m_id (id),
      m_body (body),
      m_timeout (time_out)
  {
  }

  command_t::~command_t ()
  {
    if (m_timer)
      m_timer->cancel ();
  }

  bool command_t::stream () const
  {
    return command_id_stream (m_id);
  }

  bool command_t::wrap () const
  {
    return (stream () == false);
  }

  void command_t::cancel_timeout ()
  {
    if (m_timer)
      m_timer->cancel ();
  }

  command_t::timeout_t command_t::infinity ()
  {
    return std::numeric_limits<timeout_t>::max ();
  }

  void command_t::popen (popen_ptr_t popen)
  {
    m_popen = popen;
  }

  void command_t::result (const std::string &info)
  {
    m_result_list.push_back (info);
  }

  std::string command_t::result () const
  {
    std::string res;
    for (const auto &info : m_result_list) {
      if (res.empty () == false)
        res += result_delimiter;
      res += info;
    }

    return res;
  }

}
