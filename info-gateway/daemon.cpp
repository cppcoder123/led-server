//
//
//
// To do: handle priority properly
//
#include "daemon.hpp"

namespace led_info_d
{
  daemon_t::daemon_t (const arg_t &arg)
    : m_client (arg.port, arg.host)
  {
  }

  int daemon_t::start ()
  {
    m_schedule.start ();
    return 0;
  }

  void daemon_t::stop ()
  {
    m_schedule.stop ();
    //
    for (map_t::iterator iter = m_map.begin (); iter != m_map.end (); ++iter) {
      request_ptr_t request_ptr (iter->second);
      request_ptr->action = libled::request_t::action_erase;
      m_client.send (*request_ptr);
    }
  }

  void daemon_t::schedule (const delay_t &delay, callback_t cb)
  {
    m_schedule.schedule (delay, cb);
  }

  void daemon_t::info (priority_id_t prio,
                       const libled::request_t &request)
  {
    map_t::iterator iter (m_map.find (request.tag));
    if (request.action == libled::request_t::action_insert) {
      request_ptr_t request_ptr (new libled::request_t (request));
      m_map[request.tag] = request_ptr;
    } else if (request.action == libled::request_t::action_erase) {
      if (iter != m_map.end ())
        m_map.erase (iter);
    }
    
    m_client.send (request);
  }
  
} // namespace led_info_d
