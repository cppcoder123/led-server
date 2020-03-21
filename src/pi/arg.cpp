//
//
//


#include "tclap/CmdLine.h"
#include "tclap/MultiArg.h"
#include "tclap/SwitchArg.h"
#include "tclap/ValueArg.h"

#include "arg.hpp"

namespace led_d
{

  arg_t::arg_t ()
    // : foreground (false),
    //   kill (false)
  {
  }

  bool arg_t::init (arg_t &arg, int argc, char **argv)
  {
    try {
      TCLAP::CmdLine parser ("xxx", ' ', "0.0.1");

      // TCLAP::SwitchArg
      //   foreground
      //   ("f", "foreground", "Run daemon in foreground", parser, false);
      // TCLAP::SwitchArg
      //   kill ("k", "kill", "Kill old process", parser, false);
      TCLAP::SwitchArg
        spi ("i", "spI", "Show spi messages", parser, false);
      //
      //
      //
      TCLAP::ValueArg<std::string>
        default_font ("d", "default-font", "Default font to use",
                      false, "", "font name", parser);

      TCLAP::MultiArg<std::string>
        subject_regexp_list
        ("s", "subect-regexp", "Regexp and replacement for track name",
         false, "regexp string", parser);

      parser.parse (argc, argv);

      // arg.foreground = foreground.getValue ();
      // arg.kill = kill.getValue ();
      arg.spi_msg = spi.getValue ();
      //
      arg.default_font = default_font.getValue ();

      for (auto iter = subject_regexp_list.begin ();
           iter != subject_regexp_list.end (); ++iter)
        arg.subject_regexp_list.push_back (*iter);
    }

    catch (TCLAP::ArgException &e) {
      std::cerr << "error: " << e.error () << " for arg "
                << e.argId () << std::endl;
      return false;
    }

    return true;
  }

} // namespace led_d
