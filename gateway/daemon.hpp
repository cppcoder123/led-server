//
//
//
#ifndef LED_INFO_D_DAEMON_HPP
#define LED_INFO_D_DAEMON_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "asio/asio.hpp"

#include "unix/client.hpp"
#include "unix/request.hpp"
#include "unix/response.hpp"

#include "arg.hpp"
#include "delay.hpp"
#include "priority.hpp"

namespace led_info_d
{

  class daemon_t
  {

  public:

    daemon_t (const arg_t &arg);
    ~daemon_t () {}

    asio::io_context& get_context () {return m_context;}

    int start ();
    void stop ();

    using callback_t = std::function<void (const asio::error_code &error)>;
    void schedule (const delay_t &delay, callback_t cb);
    void info (priority_id_t prio,
               const unix::request_t &request);

  private:

    void notify_connect ();
    void notify_write ();
    void notify_read (std::string &response);

    void write (const unix::request_t &request);

    asio::io_context m_context;
    asio::steady_timer m_timer;
    unix::client_t<255> m_client;

    using request_ptr_t = std::shared_ptr<unix::request_t>;
    using map_t = std::map<std::string/*tag*/, request_ptr_t>;
    map_t m_map;
    
  };
  
} // namespace led_info_d

#endif
