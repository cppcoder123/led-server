//
//
//
#ifndef UNIX_PATCH_HPP
#define UNIX_PATCH_HPP

#include <sstream>

namespace unix
{
  namespace patch
  {
    
    template <typename info_t>
    std::string to_string (const info_t &info)
    {
      std::ostringstream stream;
      stream << info;
      return stream.str ();
    }
    
  } // namespace patch
} // namespace unix

#endif
