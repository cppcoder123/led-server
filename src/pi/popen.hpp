//
//
//
#ifndef POPEN_HPP
#define POPEN_HPP

#include <sys/types.h>

#include <cstdio>
#include <csignal>
#include <string>

#include "asio.hpp"

namespace led_d
{

  class popen_t
  {
  public:
    using descriptor_t = asio::posix::stream_descriptor;

    // Achtung: it throws
    popen_t (const std::string &command, bool read, asio::io_context &context);
    popen_t () = delete;
    popen_t (const popen_t&) = delete;
    ~popen_t ();

    // 'fileno (FILE*)' call
    descriptor_t& descriptor () {return m_descriptor;}

    // Try to read smth
    // Note: ignore if 'info' is empty
    bool read (std::string &info);

    // man -S 2 kill
    bool kill (int signal);

  private:

    FILE *m_file_ptr;
    descriptor_t m_descriptor;

    pid_t m_pid;
  };

} // led_d

// list of all tracks
// mpc -f "%file%" playlist

#endif
