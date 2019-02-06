//
//
//
#ifndef UNIX_REQUEST_HPP
#define UNIX_REQUEST_HPP

#include <sstream>
#include <string>
#include <vector>

#include "format.hpp"
#include "token.hpp"

namespace unix
{

  struct request_t
  {
    enum {
      action_idle = 0,
      action_insert,
      action_erase
    };

    typedef unsigned duration_t;
    typedef unsigned action_t;
    //
    //
    request_t ();
    ~request_t () {}

    bool decode (const std::string &src,
                 const token_t::pair_vector_t &token_vector);
    std::string encode (char refsymbol) const;
    //
    //
    action_t action;
    std::string tag, format, info;
    duration_t duration;

    //static const duration_t max_duration = 10000; // 10 sec
    static const duration_t max_duration = 300000; // 5min
  };

  inline request_t::request_t ()
    : action (action_idle),
      format (format_t::encode_empty ()),
      duration (request_t::max_duration)
  {
  }

  inline bool request_t::decode (const std::string &src,
                                 const token_t::pair_vector_t &token_vector)
  {
    return
      token_t::convert (src, token_vector, action, tag, format, info, duration);
  }

  inline std::string request_t::encode (char refsymbol) const
  {
    std::string msg;
    token_t::convert (msg, refsymbol, action, tag, format, info, duration);

    return msg;
  }

  
} // namespace unix

#endif
