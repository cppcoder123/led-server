//
//
//

#include <iostream>

#include "unix/client.hpp"

#include "arg.hpp"

int main (int argc, char **argv)
{
  led::arg_t arg;
  if (led::arg_t::init (arg, argc, argv) == false) {
    std::cout << "Failed to parse command line args\n";
    return 1;
  }

  core::client_t client (arg.port, arg.host);
  
  return (client.send (arg.m_request) == true) ? 0 : 2;
}
