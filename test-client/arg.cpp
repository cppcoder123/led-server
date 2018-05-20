//
//
//


#include <iostream>

#include "tclap/CmdLine.h"
#include "tclap/SwitchArg.h"
#include "tclap/ValueArg.h"

#include "refsymbol.hpp"

#include "arg.hpp"

namespace led
{

  // arg_t::arg_t ()
  // {
  // }

  bool arg_t::init (arg_t &arg, int argc, char **argv)
  {
    try {
      TCLAP::CmdLine parser ("xxx", ' ', "0.0.1");

      TCLAP::ValueArg<libled::port_t::value_t>
        port_arg ("p", "port", "Port to listen",
                  false, libled::port_t::default_port (), "integer", parser);

      TCLAP::ValueArg<std::string>
        host_arg ("o", "host", "Host to connect",
                  false, "localhost", "string", parser);

      TCLAP::ValueArg<std::string>
        action_arg ("a", "action", "action to perform : insert/erase",
                    true, "", "string", parser);

      TCLAP::ValueArg<std::string>
        tag_arg ("t", "tag", "info tag", true, "", "string", parser);

      TCLAP::ValueArg<std::string>
        format_arg ("f", "format", "Format string for the info", false, "", "string", parser);
      
      TCLAP::ValueArg<std::string>
        info_arg ("i", "info", "info", false, "", "string", parser);

      TCLAP::ValueArg<libled::request_t::duration_t>
        duration_arg ("d", "duration", "info duration", false,
                      libled::request_t::max_duration, "unsigned", parser);
      
    
      parser.parse (argc, argv);

      arg.port = port_arg.getValue ();
      arg.host = host_arg.getValue ();
      //
      std::string buf = action_arg.getValue ();
      if ((buf != "insert") && (buf != "erase")) {
        std::cout << "Unknown action \"" << buf << "\", exiting\n";
        return false;
      }
      arg.m_request.action = (buf == "insert")
        ? libled::request_t::action_insert : libled::request_t::action_erase;
      arg.m_request.tag = libled::refsymbol_t::validate (tag_arg.getValue ());
      if (arg.m_request.tag.empty () == true) {
        std::cout << "Empty tag is not allowed, exiting\n";
        return false;
      }

      buf = format_arg.getValue ();
      if (buf.empty () == false) // Note: do not try assign "" here
        arg.m_request.format = buf;

      arg.m_request.info = libled::refsymbol_t::validate (info_arg.getValue ());
      if (arg.m_request.info.empty () == true) {
        if (arg.m_request.action == libled::request_t::action_insert) {
          std::cout << "Empty 'info' is not allowed for 'insert' action\n";
          return false;
        } else {
          // set dummy info to enable encode/decode
          arg.m_request.info = "-";
        }
      }
      arg.m_request.duration = duration_arg.getValue ();

      return true;
    }

    catch (TCLAP::ArgException &e) {
      std::cout << "error: " << e.error () << " for arg " << e.argId () << std::endl;
      return false;
    }
    
    return true;
  }
  
} // namespace led
