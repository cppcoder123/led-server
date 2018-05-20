//
//
//
#ifndef LED_D_DISPLAY_HPP
#define LED_D_DISPLAY_HPP

#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "request.hpp"
#include "response.hpp"

#include "hw.hpp"

#include "arg.hpp"
#include "render.hpp"

namespace led_d
{
  class display_t
  {

  public:

    display_t ();
    ~display_t () {}

    void start (const arg_t &arg);
    void stop ();

    typedef libled::request_t request_t;
    typedef libled::response_t response_t;
    
    void update (const request_t &request, response_t &response);
    
  private:

    bool next (request_t &info);
    bool prepare (request_t &info) const;
    void show (const request_t &info) const;
    
    arg_t m_arg;

    bool m_go_ahead;

    typedef std::unique_ptr<request_t> request_ptr_t;
    typedef std::map<std::string/*tag*/,
                     request_ptr_t/*info*/> request_map_t;
    request_map_t m_request_map;
    request_map_t::iterator m_request_iterator;

    typedef std::mutex mutex_t;
    typedef std::unique_lock<mutex_t> lock_t;
    typedef std::condition_variable condition_t;

    // protect request map & iterator
    mutable mutex_t m_mutex;
    mutable condition_t m_condition;

    render_t m_render;

    typedef std::unique_ptr<ledhw::hw_t> hw_ptr_t;
    hw_ptr_t m_hw_ptr;

  };
} // namespace led_d

#endif
