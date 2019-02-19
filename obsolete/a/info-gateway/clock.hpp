//
//
//
#ifndef LED_INFO_D_CLOCK_HPP
#define LED_INFO_D_CLOCK_HPP

#include <time.h>

#include <string>
#include <vector>

#include "daemon.hpp"

namespace led_info_d
{
  class clock_t
  {

  public:

    clock_t (daemon_t &daemon);
    ~clock_t () {}

  private:

    typedef std::vector<std::string> string_vector_t;

    void get_time ();
    void get_date ();
    void get_day ();

    static void localtime (struct tm &tm_);

    static delay_t tomorrow_delay (const struct tm &tm_);
    

    static const std::string time_prefix, time_tag;
    static const std::string date_prefix, date_tag;
    static const std::string day_tag;
    
    daemon_t &m_daemon;
  };
  
} // namespace led_info_d
#endif
