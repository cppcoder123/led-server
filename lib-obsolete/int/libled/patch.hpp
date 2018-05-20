//
//
//
#ifndef LIBLED_PATCH_HPP
#define LIBLED_PATCH_HPP

#include <sstream>

namespace libled
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
} // namespace libled

#endif
