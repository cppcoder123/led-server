/*
 *
 */

#include <csignal>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include "unix/log.hpp"

#include "popen.hpp"

namespace led_d
{

  constexpr auto pid_string = "pid";

  constexpr auto asio_buf_size = 128;

  const std::regex popen_t::m_regex ("\\s*([^: ]+)\\s*:\\s*(\\d+)\\s*");

  popen_t::popen_t (const std::string &command,
                    asio::io_context &context, bash_queue_t &queue)
    : m_file_ptr (popen (command.c_str (), "r")),
      m_descriptor (context, ((m_file_ptr != 0) ? fileno (m_file_ptr) : -1)),
      m_queue (queue),
      m_pid (0)
  {
    if (m_file_ptr == 0) {
      std::string text = "Failed to invoke command \"" + command + "\"";
      throw std::runtime_error (text);
    }
    m_descriptor.async_read_some
      (asio::buffer (m_asio_buf, m_asio_buf.size ()),
       std::bind (&popen_t::handle_read, this,
                  std::placeholders::_1, std::placeholders::_2));
  }

  popen_t::~popen_t ()
  {
    if (m_file_ptr == 0)
      return;

    m_descriptor.cancel ();

    kill (SIGKILL);

    pclose (m_file_ptr);
  }

  bool popen_t::kill (int signal)
  {
    if (m_pid == 0)
      return false;

    return (::kill (m_pid, signal) == 0) ? true : false;
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

  void popen_t::handle_read (const asio::error_code &errc, std::size_t len)
  {
    if (errc) {
      log_t::buffer_t buf;
      buf << "popen: Failed to handle read";
      log_t::error (buf);
      return;
    }

    auto begin = std::begin (m_asio_buf);
    std::string asio_tmp (begin, begin + len);
    std::string line;
    std::stringstream stream (asio_tmp);
    while (std::getline (stream, line, '\n')) {
      // log_t::buffer_t buf;
      // buf << "popen: called: " << line;
      // log_t::info (buf);
      filter (line);
      if (line.empty () == false)
        m_queue.push (line);
    }

    m_descriptor.async_read_some
      (asio::buffer (m_asio_buf, m_asio_buf.size ()),
       std::bind (&popen_t::handle_read, this,
                  std::placeholders::_1, std::placeholders::_2));
  }
} // led_d
