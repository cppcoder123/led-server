//
//
//
#ifndef UNIX_LOG_HPP
#define UNIX_LOG_HPP

#include <functional>
#include <iostream>
#include <sstream>
#include <string>

namespace unix
{
  class log_t
  {

  public:

    using buffer_t = std::ostringstream;

    static void clear (buffer_t &buf);

    static void error (const buffer_t &buf);
    static void info (const buffer_t &buf);

    static void error (const std::string &buf);
    static void info (const std::string &buf);

    // 
    using media_t = std::function<void (const char*)>;

    // If no media were provided, msgs go into std::cout
    static void init (media_t error_media, media_t info_media);

  private:

    static media_t error_media (media_t media = media_t ());
    static media_t info_media (media_t media = media_t ());

    static void log_message (const std::string &buf, media_t media, bool error);
  };

  inline void log_t::clear (buffer_t &buf)
  {
    buf.str ("");
  }

  inline void log_t::error (const buffer_t &buf)
  {
    media_t media (error_media ());
    log_message (buf.str (), media, true);
  }

  inline void log_t::info (const buffer_t &buf)
  {
    media_t media (info_media ());
    log_message (buf.str (), media, false);
  }

  inline void log_t::error (const std::string &buf)
  {
    media_t media (error_media ());
    log_message (buf, media, true);
  }

  inline void log_t::info (const std::string &buf)
  {
    media_t media (info_media ());
    log_message (buf, media, false);
  }

  inline void log_t::init (media_t error_fun, media_t info_fun)
  {
    error_media (error_fun);
    info_media (info_fun);
  }

  inline log_t::media_t log_t::error_media (media_t media)
  {
    static media_t media_var;
    if (media)
      media_var = media;

    return media_var;
  }
  
  inline log_t::media_t log_t::info_media (media_t media)
  {
    static media_t media_var;
    if (media)
      media_var = media;

    return media_var;
  }
  
  inline void log_t::log_message (const std::string &buf,
                                  media_t media, bool err)
  {
    if (media) {
      media (buf.c_str ());
      return;
    }

    std::cout << ((err == true) ? "Error" : "Info")
              << ": " << buf << std::endl;
  }

}

#endif
