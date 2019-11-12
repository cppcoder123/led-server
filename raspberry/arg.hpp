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

    using port_t = unix::port_t;

    arg_t ();
    ~arg_t () {};

    static bool init (arg_t &arg, int argc, char **argv);

    port_t::value_t port;
    bool foreground;
    bool kill;                  // kill daemon if it is running
    bool spi_msg;               // print spi messages

    std::string default_font;
    std::string device;
  };
} // namespace led_d

#endif
