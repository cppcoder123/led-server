//
//
//
#ifndef LED_D_ARG_HPP
#define LED_D_ARG_HPP

#include <string>
#include <utility>

#include "unix/port.hpp"


namespace led_d
{
  class arg_t
  {

  public:

    typedef core::port_t port_t;

    arg_t ();
    ~arg_t () {};

    static bool init (arg_t &arg, int argc, char **argv);

    port_t::value_t port;
    bool foreground;
    bool kill;                  // kill if process is running
    //std::size_t size;         // number of digits

    //std::size_t stable_delay, rolling_delay, rolling_repeat;
    //std::string font_path, default_font;
    std::string default_font;
    std::string device;
  };
} // namespace led_d

#endif
