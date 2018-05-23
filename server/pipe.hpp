//
// Communicate with arduino/openwrt/...
//
#ifndef LED_D_PIPE_HPP
#define LED_D_PIPE_HPP

#include <memory>
#include <string>

#include "device.hpp"

#include "matrix.hpp"
#include "move-queue.hpp"

namespace led_d
{
  class pipe_t
  {
  public:
    pipe_t (const std::string &device_name);
    ~pipe_t ();

    bool render (core::matrix_t &&matrix);

  private:
    move::queue_t<core::matrix_t> m_queue;
    //
    std::unique_ptr<render::device_t> m_device;
  };
} // namespace led_d

#endif
