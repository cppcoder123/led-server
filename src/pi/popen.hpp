//
//
//
#ifndef POPEN_HPP
#define POPEN_HPP

#include <cstdio>

namespace led_d
{

  class popen_t
  {
  public:
    // Achtung: it throws
    popen_t (const std::string &command, bool read);
    popen_t () = delete;
    popen_t (const popen_t&) = delete;
    ~popen_t ();

    // 'fileno (FILE*)' call
    int descriptor ();

    bool kill (int signal);
  };

} // led_d

#endif
