//
//
//
#ifndef LED_INFO_D_CLOCK_HPP
#define LED_INFO_D_CLOCK_HPP

#include <time.h>

#include <string>
#include <vector>

#include "asio/asio.hpp"

#include "info-callback.hpp"

namespace led_info_d
{
  class clock_t
  {

  public:

    clock_t (asio::io_context &context);
    ~clock_t () {}

    void init (callback_vector_t &callback_vector);

  private:

    bool get_time (std::string &info);
    bool get_date (std::string &info);
    bool get_day (std::string &info);

    void init_time ();
    void init_date_day ();
    
    static void localtime (struct tm &tm_);

    static const std::string time_prefix;
    static const std::string date_prefix;
    static const std::string suffix;

    std::string m_time_info;
    std::string m_date_info;
    std::string m_day_info;

    asio::steady_timer m_time_timer;
    asio::steady_timer m_date_day_timer;

  };
  
} // namespace led_info_d
#endif
