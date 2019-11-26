/*
 *
 */

#include <csignal>
#include <stdexcept>

#include "popen.hpp"

namespace led_d
{

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

  bool popen_t::kill (int signal)
  {
    if (m_pid == 0)
      return false;

    return (::kill (m_pid, signal) == 0) ? true : false;
  }

} // led_d
