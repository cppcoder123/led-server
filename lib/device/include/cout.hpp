//
//
//
#ifndef RENDER_COUT_HPP
#define RENDER_COUT_HPP

#include "device.hpp"

namespace render
{

  class cout_t : public device_t
  {

  public:

    cout_t () {};
    ~cout_t () {};

    bool render (const libled::matrix_t &matrix)
    {matrix.dump (); return true;}

    bool brigtness (int) {return true;}
    
  };
  
} // namespace render

#endif
