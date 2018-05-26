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

    // do not throw
    virtual bool write (const msg_t &msg) = 0;
    virtual bool read (msg_t &msg, bool block) = 0;

    // throws
    virtual void write_status (const msg_t &msg) = 0;
    virtual void read_status () = 0;
  };

} // namespace render

#endif
