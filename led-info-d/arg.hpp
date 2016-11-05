//
//
//
#ifndef LED_INFO_D_ARG_HPP
#define LED_INFO_D_ARG_HPP

#include <string>

#include "libled/port.hpp"

namespace led_info_d
{

  class arg_t
  {

  public:

    arg_t ();
    ~arg_t () {}
    
    static bool init (arg_t &arg, int argc, char **argv);

    bool foreground;
    bool kill;

    libled::port_t::value_t port;
    std::string host;
  };

} // namespace led_info_d

#endif
