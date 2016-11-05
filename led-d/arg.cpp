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
                  false, libled::port_t::default_port (), "integer", parser);
      TCLAP::SwitchArg
        foreground
        ("f", "foreground", "Run daemon in foreground", parser, false);
      TCLAP::SwitchArg
        kill ("k", "kill", "Kill old process", parser, false);
      //
      //
      //
      // TCLAP::ValueArg<std::size_t>
      //   stable_delay_arg ("s", "stable_delay", "Stable info display delay",
      //                     false, 5000, "unsigned", parser);
      // TCLAP::ValueArg<std::size_t>
      //   rolling_delay_arg ("r", "rolling_delay", "Rolling info display delay",
      //                      false, 500, "unsigned", parser);
      // TCLAP::ValueArg<std::size_t>
      //   rolling_repeat_arg ("e", "rolling_repeat", "Rolling rEpeat number",
      //                       false, 3, "unsigned", parser);
      // TCLAP::ValueArg<std::string>
      //   font_path ("o", "font-path", "Directory with fonts",
      //              true, "", "directory path", parser);
      TCLAP::ValueArg<std::string>
        default_font ("d", "default-font", "Default font to use",
                      false, "", "font name", parser);
        

      
      parser.parse (argc, argv);

      arg.port = port.getValue ();
      arg.foreground = foreground.getValue ();
      arg.kill = kill.getValue ();
      //
      //arg.font_path = font_path.getValue ();
      arg.default_font = default_font.getValue ();
      //arg.size = size_arg.getValue ();
      //arg.stable_delay = stable_delay_arg.getValue ();
      //arg.rolling_delay = rolling_delay_arg.getValue ();
      //arg.rolling_repeat = rolling_repeat_arg.getValue ();
    }

    catch (TCLAP::ArgException &e) {
      std::cerr << "error: " << e.error () << " for arg "
                << e.argId () << std::endl;
      return false;
    }
    
    return true;
  }
  
} // namespace led_d
