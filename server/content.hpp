//
//
//
#ifndef LED_D_CONTENT_HPP
#define LED_D_CONTENT_HPP

#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "asio.hpp"

#include "unix/request.hpp"
#include "unix/response.hpp"

#include "arg.hpp"
#include "render.hpp"
#include "type-def.hpp"

namespace led_d
{
  class content_t
  {

  public:

    content_t (const std::string &default_font,
               asio::io_context &io_context, mcu_queue_t &to_spi_queue);
    ~content_t ();

    void start ();
    void stop ();

    using request_t = unix::request_t;
    using response_t = unix::response_t;
    
    void update (const request_t &request, response_t &response);
    
  private:

    using request_ptr_t = std::unique_ptr<request_t> ;
    using request_map_t = std::map<std::string/*tag*/, request_ptr_t/*info*/> ;

    static constexpr std::size_t max_queue_size = 3;

    void cycle (const asio::error_code &error);
    
    bool next (request_t &info);
    bool prepare (request_t &info) const;
    void show (const request_t &info);

    const std::string m_default_font;
    asio::io_context &m_context;
    asio::steady_timer m_timer;
    mcu_queue_t &m_to_spi_queue;
    
    bool m_go;

    request_map_t m_request_map;
    request_map_t::iterator m_request_iterator;

    using mutex_t = std::mutex;
    using lock_t = std::unique_lock<mutex_t>;
    using condition_t = std::condition_variable;

    //protect request map & iterator
    mutable mutex_t m_mutex;
    mutable condition_t m_condition;

    render_t m_render;
  };
} // namespace led_d

#endif
