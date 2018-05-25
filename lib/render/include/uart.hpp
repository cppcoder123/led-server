//
//
//
#ifndef RENDER_UART_HPP
#define RENDER_UART_HPP

#include <list>
#include <cstdint>
#include <string>

#include "matrix.hpp"
#include "device-codec.hpp"

#include "device.hpp"

namespace render
{
  class uart_t : public device_t
  {
  public:
    uart_t () = delete;
    uart_t (const std::string &linux_device);
    ~uart_t ();

    using codec_t = core::device::codec_t;
    using msg_t = codec_t::msg_t;
    
    void write (const msg_t &msg) override;
    void read (msg_t &msg, bool block) override;

  private:
    void device_open ();
    void device_close ();

    //using msg_t = std::list<std::uint8_t>;
    //void send_receive_check (const msg_t &msg);

    void configure_attributes (int speed);
    void set_min_count (int min_count);

    void read_status ();
    
    const std::string m_linux_device;
    int m_descriptor;
  };
  
} // namespace render

#endif
