//
//
//
#ifndef LEDHW_HW_HTPP
#define LEDHW_HW_HTPP

#include "matrix.hpp"

namespace ledhw
{

  class hw_t
  {

  public:

    virtual ~hw_t () {}

    virtual bool start () = 0;
    virtual void stop () = 0;
    
    virtual bool render (const libled::matrix_t &info) = 0;
    virtual bool brightness (int level) = 0;

    // control power relay
    virtual bool switch_relay (bool on) = 0;
  };
  
} // namespace ledhw

#endif
