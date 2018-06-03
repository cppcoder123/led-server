//
//
//
#ifndef DEVICE_DEVICE_HPP
#define DEVICE_DEVICE_HPP

#include "device-codec.hpp"
#include "device-id.h"
#include "matrix.hpp"

namespace device
{

  class device_t
  {

  public:

    virtual ~device_t () {}

    using codec_t = core::device::codec_t;
    using msg_t = codec_t::msg_t;
    using char_t = codec_t::char_t;

    // do not throw
    virtual bool write (const msg_t &msg) = 0;
    virtual bool read (msg_t &msg, bool block) = 0;

    // throws
    virtual void write_status (const msg_t &msg) = 0;
    virtual void read_status (char_t status = ID_STATUS_OK) = 0;
  };

} // namespace device

#endif
