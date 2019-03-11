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
      idle = 0,
      insert,
      erase,                    // fixme: remove
      subscribe
    };

    using action_t = unsigned;
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
  };

  inline request_t::request_t ()
    : action (idle),
      format (format_t::encode_empty ())
  {
  }

  inline bool request_t::decode (const std::string &src,
                                 const token_t::pair_vector_t &token_vector)
  {
    return
      token_t::convert (src, token_vector, action, tag, format, info);
  }

  inline std::string request_t::encode (char refsymbol) const
  {
    std::string msg;
    token_t::convert (msg, refsymbol, action, tag, format, info);

    return msg;
  }

  
} // namespace unix

#endif
