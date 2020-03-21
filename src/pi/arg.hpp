//
//
//
#ifndef LED_D_ARG_HPP
#define LED_D_ARG_HPP

#include <list>
#include <string>
#include <utility>

namespace led_d
{
  class arg_t
  {

  public:

    arg_t ();
    arg_t (const arg_t&) = default;
    ~arg_t () = default;

    static bool init (arg_t &arg, int argc, char **argv);

    // bool kill;                  // kill daemon if it is running
    bool spi_msg;               // print spi messages

    std::string default_font;
    std::list<std::string> subject_regexp_list;
  };
} // namespace led_d

#endif
