//
//
//
#ifndef UNIX_RESPONSE_HPP
#define UNIX_RESPONSE_HPP

#include <string>

#include "token.hpp"

namespace unix
{

  struct response_t
  {
    enum {
      ok = 0,
      error,
      poll,                     // ask for data
      button                    // button press
    };

    response_t ();
    ~response_t () {}

    bool decode (const std::string &src,
                 const token_t::pair_vector_t &token_vector);
    std::string encode (char refsymbol) const;
    
    int status;                 // see enum above
    std::string string_data;
    int int_data;
  };

  //
  //
  //
  inline response_t::response_t ()
    : status (ok),
      int_data (0)
      // reason should be empty
  {
  }

  inline bool response_t::decode (const std::string &src,
                                  const token_t::pair_vector_t &token_vector)
  {
    return token_t::convert (src, token_vector, status, string_data, int_data);
  }

  inline std::string response_t::encode (char refsymbol) const
  {
    std::string msg;
    token_t::convert (msg, refsymbol, status, string_data, int_data);

    return msg;
  }
  
} // namespace unix

#endif
