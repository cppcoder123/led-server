//
//
//
#ifndef RENDER_DEVICE_HTPP
#define RENDER_DEVICE_HTPP

#include "matrix.hpp"

namespace render
{

  class device_t
  {

  public:

    virtual ~device_t () {}

    virtual bool start () = 0;
    virtual void stop () = 0;

    virtual bool render (const core::matrix_t &info) = 0;
    virtual bool brightness (int level) = 0;
  };

} // namespace render

#endif
