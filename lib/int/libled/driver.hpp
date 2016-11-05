//
//
//
#ifndef LIBLED_DRIVER_HPP
#define LIBLED_DRIVER_HPP

#include "libled/matrix.hpp"

namespace libled
{
  
  class driver_t
  {

  public:

    virtual ~driver_t () {}

    virtual bool render (const matrix_t &info) = 0;

    virtual bool change_brightness (int level) = 0;
    
  };
  
} // namespace libled

#endif
