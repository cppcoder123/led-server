//
//
//
#include "tclap/CmdLine.h"
#include "tclap/SwitchArg.h"
#include "tclap/ValueArg.h"

#include "arg.hpp"

namespace led_info_d
{
  arg_t::arg_t ()
    : foreground (false),
      kill (false),
      port ()
  {
  }

  bool arg_t::init (arg_t &arg, int argc, char **argv)
  {
    try {
      TCLAP::CmdLine parser ("xxx", ' ', "0.0.1");

      TCLAP::SwitchArg foreground
        ("f", "foreground", "Run daemon in foreground", parser, false);
      TCLAP::SwitchArg kill
        ("k", "kill", "Kill old process", parser, false);

      TCLAP::ValueArg<core::port_t::value_t> port
        ("p", "port", "Port to connect",
         false, core::port_t::default_port (), "integer", parser);
      TCLAP::ValueArg<std::string> host
        ("o", "host", "Host to connect",
         false, "localhost", "hostname", parser);

      parser.parse (argc, argv);

      arg.foreground = foreground.getValue ();
      arg.kill = kill.getValue ();
      arg.port = port.getValue ();
      arg.host = host.getValue ();
    }
    catch (TCLAP::ArgException &e) {
      std::cout << "error: " << e.error () << std::endl;
      return false;
    }

    return true;
  }
  
} // namespace led_info_d
