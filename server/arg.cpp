//
//
//


#include "tclap/CmdLine.h"
#include "tclap/SwitchArg.h"
#include "tclap/ValueArg.h"

#include "arg.hpp"

namespace led_d
{

  arg_t::arg_t ()
    : port (),
      foreground (false),
      kill (false)
  {
  }

  bool arg_t::init (arg_t &arg, int argc, char **argv)
  {
    try {
      TCLAP::CmdLine parser ("xxx", ' ', "0.0.1");

      TCLAP::ValueArg<port_t::value_t>
        port ("p", "port", "Port to listen",
                  false, unix::port_t::default_port (), "integer", parser);
      TCLAP::SwitchArg
        foreground
        ("f", "foreground", "Run daemon in foreground", parser, false);
      TCLAP::SwitchArg
        kill ("k", "kill", "Kill old process", parser, false);
      //
      //
      //
      TCLAP::ValueArg<std::string>
        default_font ("d", "default-font", "Default font to use",
                      false, "", "font name", parser);
      TCLAP::ValueArg<std::string>
        device ("s", "device", "Render device, e.g \"/dev/ttyXXX\"",
                    true, "", "dev-name", parser);

      parser.parse (argc, argv);

      arg.port = port.getValue ();
      arg.foreground = foreground.getValue ();
      arg.kill = kill.getValue ();
      //
      arg.default_font = default_font.getValue ();
      arg.device = device.getValue ();
    }

    catch (TCLAP::ArgException &e) {
      std::cerr << "error: " << e.error () << " for arg "
                << e.argId () << std::endl;
      return false;
    }

    return true;
  }

} // namespace led_d
