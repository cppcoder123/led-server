//
//
//
#ifndef LED_ARG_HPP
#define LED_ARG_HPP

#include <string>
#include <utility>

#include "unix/port.hpp"
#include "unix/request.hpp"


namespace led
{
  class arg_t
  {

  public:

    arg_t () {};
    ~arg_t () {};

    static bool init (arg_t &arg, int argc, char **argv);

    unix::port_t::value_t port;

    std::string host;

    unix::request_t m_request;
  };
} // namespace led_d

#endif
