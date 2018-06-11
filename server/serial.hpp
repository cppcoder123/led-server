//
//
//
#ifndef LED_D_SERIAL_HPP
#define LED_D_SERIAL_HPP

#include <bitset>
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

    bool ready () const {return m_flag.test (bit_ready);};
    
    // incoming info : external communication
    void bind (read_t read_cb, write_t write_cb);
    // outcoming info : async write
    bool write (const msg_t &msg);

  private:

    using char_t = codec_t::char_t;

    static constexpr std::size_t header_left_size = 2;
    static constexpr std::size_t header_right_size = 2;
    static constexpr std::size_t read_buffer_size = 127;
    static constexpr std::size_t write_buffer_size = 255;

    static constexpr std::size_t bit_ready = 0;
    static constexpr std::size_t bit_pending_read = 1;
    static constexpr std::size_t bit_pending_write = 2;
    static constexpr std::size_t bit_size = 3;

    void private_write (const msg_t &msg);

    // fixme: check we are not wait for pending read
    //   from prev call
    void asio_read (const asio::error_code &code,
                    std::size_t bytes_stransferred);
    void asio_write (const asio::error_code &code,
                     std::size_t bytes_stransferred,
                     std::size_t bytes_expected);

    bool decode_header (msg_t &msg);

    void decode_initial (msg_t &msg);


    read_t m_read;
    write_t m_write;

    std::bitset<bit_size> m_flag;

    using asio_callback_t
    = std::function<void (const asio::error_code &code,
                          std::size_t bytes_stransferred)>;

    asio_callback_t m_asio_read;

    char_t m_read_buffer[read_buffer_size];
    char_t m_write_buffer[write_buffer_size];

    asio::serial_port m_port;

    enum state_t {
      state_first,
      state_hello = state_first,
      state_handshake,
      state_pixel_delay,
      state_phrase_delay,
      state_stable_delay,
      state_brightness,
      state_last,
      state_init = state_last
    };
    state_t m_state;
  };
  
} // namespace led_d

#endif
