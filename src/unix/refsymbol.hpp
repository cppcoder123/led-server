//
//
//
#ifndef UNIX_REFSYMBOL_HPP
#define UNIX_REFSYMBOL_HPP

#include <string>

namespace unix
{

  struct refsymbol_t
  {
    static const char value = '\\';
    static const char replace = '/';
    //

    // replace invalid symbols
    static std::string validate (const std::string &src);
  };

  //
  //
  //
  inline std::string refsymbol_t::validate (const std::string &src)
  {
    std::string dst (src);
    for (std::size_t i = 0; i < dst.size (); ++i)
      if (dst[i] == value)
        dst[i] = replace;

    return dst;
  }

  
} // namespace unix


#endif