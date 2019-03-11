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

    // typedef std::vector<std::string> string_vector_t;

    // void get_time (const asio::error_code &error);
    // void get_date (const asio::error_code &error);
    // void get_day (const asio::error_code &error);

    // static void localtime (struct tm &tm_);

    // static delay_t tomorrow_delay (const struct tm &tm_);
    

    static const std::string time_prefix;
    static const std::string date_prefix;

    // static const std::string time_prefix, time_tag;
    // static const std::string date_prefix, date_tag;
    // static const std::string day_tag;
    
    //daemon_t &m_daemon;
  };
  
} // namespace led_info_d
#endif
