//
//
//
#include <functional>

#include "schedule.hpp"

namespace led_info_d
{
  void schedule_t::start ()
  {
    m_thread = std::thread (&schedule_t::loop, this);
  }

  void schedule_t::stop ()
  {
    m_go_ahead = false;
    m_condition.notify_one ();
    m_thread.join ();
  }

  void schedule_t::schedule (const delay_t &delay, callback_t cb)
  {
    // 1. calculate future time point
    time_point_t point (std::chrono::system_clock::now ());
    point += delay;
    // + delay
    {
      // 2. insert into map
      guard_t g (m_mutex);
      m_map.insert (map_t::value_type (point, cb));
    }
    
    // 3. notify own thread
    m_condition.notify_one ();
  }

  void schedule_t::loop ()
  {
    while (m_go_ahead == true) {
      callback_t callback;
      {
        std::unique_lock<mutex_t> lock (m_mutex);
        map_t::iterator iter (m_map.begin ());
        if (iter == m_map.end ()) {
          m_condition.wait (lock);
          continue;
        }
        const time_point_t now (std::chrono::system_clock::now ());
        const time_point_t &future_point (iter->first);
        if (now < future_point) {
          m_condition.wait_for (lock, future_point - now);
          continue;
        }
        callback = iter->second;
        m_map.erase (iter);
      }
      callback ();
    }
  }
  
} // namespace led_info_d
