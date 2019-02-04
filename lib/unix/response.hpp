//
//
//
#ifndef CORE_RESPONSE_HPP
#define CORE_RESPONSE_HPP

#include <string>

#include "token.hpp"

namespace core
{

  struct response_t
  {

    response_t ();
    ~response_t () {}

    bool decode (const std::string &src,
                 const token_t::pair_vector_t &token_vector);
    std::string encode (char refsymbol) const;
    
    static const int status_ok = 0;
    static const int status_error = 1;
    
    int status;
    std::string reason;
  };

  //
  //
  //
  inline response_t::response_t ()
    : status (status_ok)
      // reason should be empty
  {
  }

  inline bool response_t::decode (const std::string &src,
                                  const token_t::pair_vector_t &token_vector)
  {
    return token_t::convert (src, token_vector, status, reason);
  }

  inline std::string response_t::encode (char refsymbol) const
  {
    std::string msg;
    token_t::convert (msg, refsymbol, status, reason);

    return msg;
  }
  
} // namespace core

#endif
