//
//
//
#ifndef POPEN_HPP
#define POPEN_HPP

#include <sys/types.h>

#include <cstdio>
#include <csignal>
#include <functional>
#include <regex>
#include <string>

#include "asio.hpp"

namespace led_d
{

  class popen_t// : public std::enable_shared_from_this<popen_t>
  {
  public:
    using info_arrived_t = std::function<void (const std::string&)>;
    using error_occurred_t = std::function<void ()>;

    // Attention: it throws
    popen_t (const std::string &cmd_body, asio::io_context &context,
             info_arrived_t info_arrived, error_occurred_t error_occurred);
    popen_t () = delete;
    popen_t (const popen_t&) = delete;
    ~popen_t ();

    // man -S 2 kill
    bool kill (int signal);

    static bool split (const std::string &src,
                       std::string &prefix, std::string &suffix,
                       const std::string &pattern);

    static bool split (const std::string &src,
                       std::string &prefix, std::string &suffix,
                       const std::regex &regex);

  private:
    using descriptor_t = asio::posix::stream_descriptor;

    static bool split (const std::string &src,
                       std::string &prefix, std::string &suffix);

    void filter (std::string &info);

    void handle_read (const asio::error_code &error, std::size_t len);

    FILE *m_file_ptr;
    descriptor_t m_descriptor;

    info_arrived_t m_info_arrived;
    error_occurred_t m_error_occured;

    pid_t m_pid;

    static constexpr auto asio_buf_size = 128;
    std::array<char, asio_buf_size> m_asio_buf;

    static const std::regex m_regex;
  };

} // led_d

// list of all tracks
// mpc -f "%file%" playlist

#endif
