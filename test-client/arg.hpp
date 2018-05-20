//
//
//
#ifndef LED_ARG_HPP
#define LED_ARG_HPP

#include <string>
#include <utility>

#include "port.hpp"
#include "request.hpp"


namespace led
{
  class arg_t
  {

  public:

    arg_t () {};
    ~arg_t () {};

    static bool init (arg_t &arg, int argc, char **argv);

    core::port_t::value_t port;

    std::string host;

    core::request_t m_request;
  };
} // namespace led_d

#endif
