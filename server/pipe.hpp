//
// Communicate with arduino/openwrt/...
//
#ifndef LED_D_PIPE_HPP
#define LED_D_PIPE_HPP

#include <limits>
#include <memory>
#include <string>
#include <list>

#include "device-codec.hpp"

#include "block.hpp"
#include "matrix.hpp"
#include "mutex-queue.hpp"
#include "serial.hpp"

namespace led_d
{
  class pipe_t
  {
  public:
    pipe_t (serial_t &serial);
    ~pipe_t ();

    bool render (const core::matrix_t &matrix);

  private:
    using codec_t = core::device::codec_t;
    using char_t = codec_t::char_t;
    using msg_t = codec_t::msg_t;

    using queue_t = mutex::queue_t<msg_t>;

    void write ();
    
    void decode (codec_t::msg_t &msg);

    codec_t::char_t get_serial_id ();
    
    queue_t m_write_queue;
    //
    serial_t &m_device;
    codec_t::char_t m_serial_id;
    //
    block_t m_block;
  };

} // namespace led_d

#endif
