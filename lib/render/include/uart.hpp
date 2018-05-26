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

    bool write (const codec_t::msg_t &msg) override;
    bool read (codec_t::msg_t &msg, bool block) override;

    std::string get_error ();

    void read_status ();
    void write_status (const msg_t &msg);

  private:

    void device_open ();
    void device_close ();

    void configure_attributes (int speed);
    void set_min_count (int min_count);

    void unwrap_fill (codec_t::msg_t &msg,
                      bool &started, std::size_t &msg_size);
    
    static constexpr std::size_t io_max_size = 80;
    
    const std::string m_linux_device;
    int m_descriptor;
    std::string m_error;

    codec_t::msg_t m_message_buffer;
  };
  
} // namespace render

#endif
