//
//
//
#ifndef POPEN_HPP
#define POPEN_HPP

#include <sys/types.h>

#include <cstdio>
#include <csignal>
#include <regex>
#include <string>

#include "asio.hpp"

#include "bash-queue.hpp"

namespace led_d
{

  class popen_t// : public std::enable_shared_from_this<popen_t>
  {
  public:
    // Achtung: it throws
    popen_t (const std::string &command,
             asio::io_context &context, bash_queue_t &queue);
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
    bash_queue_t &m_queue;

    pid_t m_pid;

    static constexpr auto asio_buf_size = 128;
    std::array<char, asio_buf_size> m_asio_buf;

    static const std::regex m_regex;
  };

} // led_d

// list of all tracks
// mpc -f "%file%" playlist

#endif
