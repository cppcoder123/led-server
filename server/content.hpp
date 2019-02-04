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
//#include "pipe.hpp"
#include "render.hpp"
//#include "serial.hpp"

namespace led_d
{
  class content_t
  {

  public:

    content_t (asio::io_context &io_context);
    ~content_t ();

    void start (const arg_t &arg/*, serial_t &serial*/);
    void stop ();

    using request_t = core::request_t;
    using response_t = core::response_t;
    
    void update (const request_t &request, response_t &response);
    
  private:

    static constexpr std::size_t max_queue_size = 3;

    bool next (request_t &info);
    bool prepare (request_t &info) const;
    void show (const request_t &info);
    
    arg_t m_arg;

    bool m_go_ahead;

    using request_ptr_t = std::unique_ptr<request_t> ;
    using request_map_t = std::map<std::string/*tag*/,
                                   request_ptr_t/*info*/> ;
    request_map_t m_request_map;
    request_map_t::iterator m_request_iterator;

    using mutex_t = std::mutex;
    using lock_t = std::unique_lock<mutex_t>;
    using condition_t = std::condition_variable;

    // protect request map & iterator
    mutable mutex_t m_mutex;
    mutable condition_t m_condition;

    render_t m_render;

    //std::unique_ptr<pipe_t> m_pipe_ptr;
  };
} // namespace led_d

#endif
