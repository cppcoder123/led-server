//
//
//

#include <time.h>

#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "unix/log.hpp"
#include "unix/patch.hpp"

#include "clock.hpp"
#include "daemon.hpp"

namespace led_info_d
{
  namespace {
    namespace chrono = std::chrono;
    using string_vector_t = std::vector<std::string>;
  }

  const std::string clock_t::time_prefix ("T ");
  const std::string clock_t::date_prefix ("D ");
  const std::string clock_t::suffix ("  ");

  clock_t::clock_t (asio::io_context &context)
    : m_time_timer (context),
      m_date_day_timer (context)
  {
  }

  void clock_t::init (callback_vector_t &callback_vector)
  {
    callback_vector.push_back
      (std::bind (&clock_t::get_time, this, std::placeholders::_1));
    callback_vector.push_back
      (std::bind (&clock_t::get_date, this, std::placeholders::_1));
    callback_vector.push_back
      (std::bind (&clock_t::get_day, this, std::placeholders::_1));

    //
    //
    //
    init_time ();
    init_date_day ();
  }

  bool clock_t::get_time (std::string &info)
  {
    info = m_time_info;

    return true;
  }

  bool clock_t::get_date (std::string &info)
  {
    info = m_date_info;

    return true;
  }

  bool clock_t::get_day (std::string &info)
  {
    info = m_day_info;

    return true;
  }

  void clock_t::init_time ()
  {
    struct tm tm_;
    localtime (tm_);

    std::string zero = (tm_.tm_min < 10) ? "0" : "";
    m_time_info = time_prefix + unix::patch::to_string (tm_.tm_hour) + '-'
      + zero + unix::patch::to_string (tm_.tm_min) + suffix;

    m_time_timer.expires_at
      (chrono::steady_clock::now () + chrono::seconds (60 - tm_.tm_sec));
    m_time_timer.async_wait (std::bind (&clock_t::init_time, this));
  }

  void clock_t::init_date_day ()
  {
    struct tm tm_;
    localtime (tm_);

    static const string_vector_t month_vector =
      {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
       "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    m_date_info = date_prefix + unix::patch::to_string (tm_.tm_mday)
      + ' ' + month_vector[tm_.tm_mon] + suffix;

    static const string_vector_t day_vector =
      {"Sunday", "Monday", "Tuesday",
       "Wednesday", "Thursday", "Friday", "Saturday"};
    m_day_info = day_vector[tm_.tm_wday] + suffix;

    m_date_day_timer.expires_at
      (chrono::steady_clock::now ()
       + chrono::seconds
       (61 - tm_.tm_sec + (60 - tm_.tm_min) * 60 + (24 - tm_.tm_hour) * 3600));

    m_date_day_timer.async_wait (std::bind (&clock_t::init_date_day, this));
  }

  void clock_t::localtime (struct tm &tm_)
  {
    chrono::system_clock::time_point now (chrono::system_clock::now ());
    time_t tt = chrono::system_clock::to_time_t (now);
    localtime_r (&tt, &tm_);
  }

} // namespace led_info_d
