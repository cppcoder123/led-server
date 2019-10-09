//
//
//
#ifndef LED_INFO_D_SCHEDULE_HPP
#define LED_INFO_D_SCHEDULE_HPP

#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>

#include "delay.hpp"

namespace led_info_d
{

  class schedule_t
  {

  public:

    schedule_t ()
      : m_go_ahead (true) {}
    ~schedule_t () {}

    void start ();
    void stop ();

    typedef std::function<void (void)> callback_t;
    void schedule (const delay_t &delay, callback_t cb);

  private:

    void loop ();
    
    bool m_go_ahead;
    std::thread m_thread;

    typedef std::mutex mutex_t;
    typedef std::lock_guard<mutex_t> guard_t;

    mutex_t m_mutex;
    std::condition_variable m_condition;
    
    typedef std::chrono::system_clock::time_point time_point_t;

    typedef std::multimap<time_point_t, callback_t> map_t;
    map_t m_map;
  };
  
} // namespace led_info_d
#endif
