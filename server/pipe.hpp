//
// Communicate with arduino/openwrt/...
//
#ifndef LED_D_PIPE_HPP
#define LED_D_PIPE_HPP

#include <memory>
#include <string>
#include <list>

#include "device.hpp"

#include "matrix.hpp"
#include "mutex-queue.hpp"

namespace led_d
{
  class pipe_t
  {
  public:
    pipe_t (const std::string &device_name);
    ~pipe_t ();

    bool render (const core::matrix_t &matrix);

  private:
    using uchar_t = unsigned char;
    using vector_t = std::list<uchar_t>;
    using queue_t = mutex::queue_t<vector_t>;
    
    queue_t m_write_queue;
    queue_t m_read_queue;
    //
    std::unique_ptr<render::device_t> m_device;
  };
} // namespace led_d

#endif
