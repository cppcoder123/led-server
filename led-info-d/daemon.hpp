//
//
//
#ifndef LED_INFO_D_DAEMON_HPP
#define LED_INFO_D_DAEMON_HPP

#include <functional>
#include <memory>

#include "libled/request.hpp"
#include "libled/client.hpp"

#include "arg.hpp"
#include "delay.hpp"
#include "priority.hpp"
#include "schedule.hpp"

namespace led_info_d
{

  class daemon_t
  {

  public:

    daemon_t (const arg_t &arg);
    ~daemon_t () {}

    int start ();
    void stop ();

    typedef schedule_t::callback_t callback_t;
    
    void schedule (const delay_t &delay, callback_t cb);
    void info (priority_id_t prio,
               const libled::request_t &request);

  private:

    schedule_t m_schedule;

    libled::client_t m_client;

    typedef std::shared_ptr<libled::request_t> request_ptr_t;
    typedef std::map<std::string/*tag*/, request_ptr_t> map_t;
    map_t m_map;
    
  };
  
} // namespace led_info_d

#endif
