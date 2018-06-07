//
//
//
#ifndef DEVICE_UART_HPP
#define DEVICE_UART_HPP

#include <list>
#include <cstdint>
#include <string>

#include "matrix.hpp"
#include "device-codec.hpp"

#include "device.hpp"

namespace device
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

    void read_status (codec_t::char_t status = ID_STATUS_OK);
    void write_status (const msg_t &msg);

  private:

    void device_open ();
    void device_close ();

    void configure_attributes (int speed);
    void set_min_count (int min_count);

    bool read_decode (codec_t::msg_t &msg);

    bool write_tty (codec_t::msg_t &&msg);
    bool read_tty ();

    
    static constexpr std::size_t max_write_size = 255;
    static constexpr std::size_t max_read_size = 80;
    //
    static constexpr std::size_t header_left_size = 2;
    static constexpr std::size_t header_right_size = 2;
    
    const std::string m_linux_device;
    int m_descriptor;
    std::string m_error;

    codec_t::msg_t m_message_buffer;
  };
  
} // namespace device

#endif
