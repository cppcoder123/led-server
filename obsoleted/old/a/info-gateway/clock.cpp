//
//
//

#include <time.h>

#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>

#include "unix/patch.hpp"

#include "clock.hpp"
#include "daemon.hpp"
#include "delay.hpp"

namespace led_info_d
{
  const std::string clock_t::time_tag ("clock-time");
  const std::string clock_t::date_tag ("clock-date");
  const std::string clock_t::day_tag ("clock-day");

  const std::string clock_t::time_prefix ("T ");
  const std::string clock_t::date_prefix ("D ");

  clock_t::clock_t (daemon_t &daemon)
    : m_daemon (daemon)
  {
    m_daemon.schedule (delay_t (0),
                       std::bind (&clock_t::get_time, this));
    m_daemon.schedule (delay_t (0),
                       std::bind (&clock_t::get_date, this));
    m_daemon.schedule (delay_t (0),
                       std::bind (&clock_t::get_day, this));
  }

  void clock_t::get_time ()
  {
    struct tm tm_;
    localtime (tm_);

    unix::request_t request;

    request.action = unix::request_t::action_insert;
    request.tag = time_tag;
    std::string zero = (tm_.tm_min < 10) ? "0" : "";
    request.info = time_prefix + unix::patch::to_string (tm_.tm_hour) + '-'
      + zero + unix::patch::to_string (tm_.tm_min);

    m_daemon.info (priority_t::medium, request);
    
    m_daemon.schedule (delay_t (60 - tm_.tm_sec),
                       std::bind (&clock_t::get_time, this));
  }

  void clock_t::get_date ()
  {
    struct tm tm_;
    localtime (tm_);
    static const string_vector_t month_vector =
      {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
       "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    std::string info (date_prefix + unix::patch::to_string (tm_.tm_mday)
                      + ' ' + month_vector[tm_.tm_mon]);

    unix::request_t request;
    request.action = unix::request_t::action_insert;
    request.tag = date_tag;
    request.info = info;

    m_daemon.info (priority_t::low, request);

    m_daemon.schedule (tomorrow_delay (tm_), std::bind (&clock_t::get_date, this));
  }

  void clock_t::get_day ()
  {
    struct tm tm_;
    localtime (tm_);
    static const string_vector_t day_vector =
      {"Sunday", "Monday", "Tuesday",
       "Wednesday", "Thursday", "Friday", "Saturday"};

    unix::request_t request;
    request.action = unix::request_t::action_insert;
    request.tag = day_tag;
    request.info = day_vector[tm_.tm_wday];

    m_daemon.info (priority_t::low, request);

    m_daemon.schedule (tomorrow_delay (tm_),
                       std::bind (&clock_t::get_day, this));
  }

  void clock_t::localtime (struct tm &tm_)
  {
    std::chrono::system_clock::time_point now (std::chrono::system_clock::now ());
    time_t tt = std::chrono::system_clock::to_time_t (now);
    localtime_r (&tt, &tm_);
  }

  delay_t clock_t::tomorrow_delay (const struct tm &tm_)
  {
    delay_t delay
      (61 - tm_.tm_sec + (60 - tm_.tm_min) * 60 + (24 - tm_.tm_hour) * 3600);
    return delay;
  }
  
} // namespace led_info_d
