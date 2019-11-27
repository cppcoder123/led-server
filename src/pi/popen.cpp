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
                    bool read, asio::io_context &context)
    : m_file_ptr (popen (command.c_str (), (read == true) ? "r" : "w")),
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
    if (m_pid != 0)
      // Set pid only once
      return;

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
                       std::string &prefix, std::string &suffix)
  {
    std::smatch result;

    if (std::regex_match (src, result, m_regex) == false)
      return false;

    if (result.size () < 3)
      return false;

    // [0] is whole expression
    prefix = result[1];
    suffix = result[2];

    return true;
  }

} // led_d
