//
//
//
#ifndef COUT_DRIVER_HPP
#define COUT_DRIVER_HPP

#include "libled/driver.hpp"

namespace cout
{
  class driver_t : public libled::driver_t
  {

  public:

    driver_t () {}
    ~driver_t () {}

    bool render (const libled::matrix_t &matrix)
    {matrix.dump (); return true;}

    bool change_brightness (int)
    {return true;}
    
  };
}

#endif
