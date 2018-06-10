//
//
//
#ifndef LED_D_SERIAL_HPP
#define LED_D_SERIAL_HPP

#include <functional>
#include <string>
#include <vector>

#include "asio.hpp"

#include "device-codec.hpp"

namespace led_d
{

  class serial_t
  {
  public:
    using codec_t = core::device::codec_t;
    using msg_t = codec_t::msg_t;
    
    // read & write-complete callbacks
    using read_t = std::function<void (msg_t &msg)>;
    using write_t = std::function<void ()>;
    
    serial_t (asio::io_service &service, const std::string &device);
    serial_t (const serial_t&) = delete;
    ~serial_t () = default;

    bool ready () const {return m_ready;};
    
    // incoming info : external communication
    void bind (read_t read_cb, write_t write_cb);
    // outcoming info : async write
    bool write (const msg_t &msg);

  private:

    using char_t = codec_t::char_t;

    static constexpr std::size_t header_left_size = 2;
    static constexpr std::size_t header_right_size = 2;
    static constexpr std::size_t buffer_size = 255;
    static constexpr speed_t port_speed = 500000;

    void configure_tty ();

    void private_write (const msg_t &msg);
    
    // void transfer_check (bool write, std::size_t expected,
    //                      const asio::error_code &error,
    //                      std::size_t actual);

    void asio_read (const asio::error_code &code,
                    std::size_t bytes_stransferred);
    void asio_write (const asio::error_code &code,
                     std::size_t bytes_stransferred);
    
    read_t m_read;
    write_t m_write;

    bool m_ready;

    using asio_callback_t
    = std::function<void (const asio::error_code &code,
                          std::size_t bytes_stransferred)>;

    asio_callback_t m_asio_read;
    asio_callback_t m_asio_write;

    char_t m_read_buffer[buffer_size];
    char_t m_write_buffer[buffer_size];

    asio::serial_port m_port;
  };
  
} // namespace led_d

#endif
