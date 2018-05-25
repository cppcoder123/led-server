//
//
//
#ifndef RENDER_DEVICE_HTPP
#define RENDER_DEVICE_HTPP

#include "matrix.hpp"
#include "device-codec.hpp"

namespace render
{

  class device_t
  {

  public:

    virtual ~device_t () {}

    using msg_t = core::device::codec_t::msg_t;

    virtual void write (const msg_t &msg) = 0;
    virtual void read (msg_t &msg, bool block) = 0;
  };

} // namespace render

#endif
