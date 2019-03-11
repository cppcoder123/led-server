//
//
//

#ifndef CONTENT_HPP
#define CONTENT_HPP

#include "asio/asio.hpp"

#include "unix/request.hpp"

#include "clock.hpp"
#include "info-callback.hpp"

namespace led_info_d
{

  class content_t
  {

  public:

    content_t () = delete;
    content_t (const content_t&) = delete;

    using write_t = std::function<void (const unix::request_t& /*info*/)>;
    content_t (asio::io_context &context, write_t write);
    ~content_t () {};

    void init ();

    void push ();

  private:

    callback_vector_t m_info;
    std::size_t m_current;

    clock_t m_clock;

    write_t m_write;
    
  };

} // namespace led_info_d

#endif
