//
//
//
#ifndef LEDHW_COUT_HPP
#define LEDHW_COUT_HPP

#include "ledhw/hw.hpp"

namespace ledhw
{

  class cout_t : public hw_t
  {

  public:

    cout_t () {};
    ~cout_t () {};

    bool render (const libled::matrix_t &matrix)
    {matrix.dump (); return true;}

    bool brigtness (int) {return true;}
    
  };
  
} // namespace ledhw

#endif
