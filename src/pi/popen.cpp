/*
 *
 */

#include <csignal>
#include <sstream>
#include <stdexcept>

#include "popen.hpp"

namespace led_d
{

  static auto pid_string = "pid";

  const std::regex popen_t::m_regex ("\\s*(\\w+)\\s*:\\s*(\\w+)\\s*");

  popen_t::popen_t (const std::string &command,
                    bool read_only, asio::io_context &context)
    : m_file_ptr (popen (command.c_str (), (read_only == true) ? "r" : "w")),
      m_descriptor (context, ((m_file_ptr != 0) ? fileno (m_file_ptr) : -1)),
      m_pid (0)
  {
    if (m_file_ptr == 0) {
      std::string text = "Failed to invoke command \"" + command + "\"";
      throw std::runtime_error (text);
    }
  }

  popen_t::~popen_t ()
  {
    if (m_file_ptr == 0)
      return;

    kill (SIGKILL);

    pclose (m_file_ptr);
  }

  bool popen_t::read (std::string &info)
  {
    info = "";
    bool status = true;

    // fixme: should be rewritten without FILE* but using descriptor
    // with asio
    do {
      char ch = fgetc (m_file_ptr);
      if (ch == EOF) {
        status = false;
        break;
      } else if (ch == '\n') {
        info.swap (m_buffer);
        break;
      } else {
        m_buffer += ch;
      }
    } while (1);

    if (info.empty () == true)
      return status;

    if (m_pid == 0)
      filter (info);

    // info wasn't empty, so status can't be false
    return true;
  }

  bool popen_t::kill (int signal)
  {
    if (m_pid == 0)
      return false;

    return (::kill (m_pid, signal) == 0) ? true : false;
  }

  void popen_t::filter (std::string &info)
  {
    std::string prefix, suffix;
    if (split (info, prefix, suffix) == false)
      return;

    if (prefix != pid_string)
      return;

    info = "";

    std::istringstream stream (suffix);
    stream >> m_pid;
    // Note: Do we need a check here?
  }

  bool popen_t::split (const std::string &src,
                       std::string &prefix, std::string &suffix,
                       const std::string &pattern)
  {
    std::regex regex (pattern);

    return split (src, prefix, suffix, regex);
  }

  bool popen_t::split (const std::string &src,
                       std::string &prefix, std::string &suffix)
  {
    return split (src, prefix, suffix, m_regex);
  }

  bool popen_t::split (const std::string &src,
                       std::string &prefix, std::string &suffix,
                       const std::regex &regex)
  {
    std::smatch result;

    if (std::regex_match (src, result, regex) == false)
      return false;

    if (result.size () < 3)
      return false;

    // [0] is whole expression
    prefix = result[1];
    suffix = result[2];

    return true;
  }

} // led_d
