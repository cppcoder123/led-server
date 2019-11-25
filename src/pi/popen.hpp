//
//
//
#ifndef POPEN_HPP
#define POPEN_HPP

#include <cstdio>
#include <string>

#include "asio.hpp"

namespace led_d
{

  class popen_t
  {
  public:
    // Achtung: it throws
    popen_t (const std::string &command, bool read, asio::io_context &context);
    popen_t () = delete;
    popen_t (const popen_t&) = delete;
    ~popen_t ();

    // 'fileno (FILE*)' call
    asio::posix::stream_descriptor descriptor ();

    // Try to read smth
    // Note: ignore if it is empty
    std::string read ();

    // man -S 2 kill
    bool kill (int signal);
  };

} // led_d

// list of all tracks
// mpc -f "%file%" playlist

#endif
