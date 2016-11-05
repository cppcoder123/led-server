//
//
//
#ifndef LIBLED_CODEC_HPP
#define LIBLED_CODEC_HPP

#include <sstream>
#include <string>
#include <vector>

#include "token.hpp"


namespace libled
{

  template <typename refsymbol_t, typename info_t>
  class codec_t
  {

  public:

    static const std::size_t max_size = 1024;
    static const std::size_t header_size = 4;

    
    // decode header
    static bool decode (const std::string &src,
                        std::string &dst, std::string &rest);
    static bool decode (const std::string &src, std::size_t &msg_size);

    // body
    static bool decode (const std::string &src, info_t &dst);

    static bool encode (const info_t &src, std::string &dst);

  };

  //
  //
  //

  template <typename refsymbol_t, typename info_t>
  bool codec_t<refsymbol_t, info_t>::
  decode (const std::string &src, std::string &dst, std::string &rest)
  {
    token_t::pair_vector_t pair_vector;
    if (token_t::tokenize (pair_vector, src, refsymbol_t::value, true) == false)
      return false;
    std::size_t size = 0;
    if (token_t::convert (src, pair_vector, size) == false)
      return false;

    token_t::position_pair_t pair (pair_vector[0]);
    std::size_t dst_start = pair.first + pair.second + 1;

    if (dst_start + size > src.size ())
      return false;

    dst = src.substr (dst_start, size);

    rest = ((dst_start + size) < src.size ())
      ? src.substr (dst_start + size) : std::string ();

    return true;
  }
    
  template <typename refsymbol_t, typename info_t>
  bool codec_t<refsymbol_t, info_t>::
  decode (const std::string &src, std::size_t &msg_size)
  {
    token_t::pair_vector_t pair_vector;
    if (token_t::tokenize (pair_vector, src, refsymbol_t::value, true) == false)
      return false;
    return token_t::convert (src, pair_vector, msg_size);
  }

  template <typename refsymbol_t, typename info_t>
  bool codec_t<refsymbol_t, info_t>::
  decode (const std::string &src, info_t &dst)
  {
    token_t::pair_vector_t pair_vector;
    if (token_t::tokenize (pair_vector, src, refsymbol_t::value) == false)
      return false;
    return dst.decode (src, pair_vector);
  }
  

  template <typename refsymbol_t, typename info_t>
  bool codec_t<refsymbol_t, info_t>::
  encode (const info_t &src, std::string &dst)
  {
    std::string tmp = src.encode (refsymbol_t::value);
    if (tmp.size () > max_size)
      return false;
    
    std::string prefix
      ((tmp.size () < 10) ? "00" : (tmp.size () < 100) ? "0" : "");
    dst = prefix + std::to_string (tmp.size ()) + refsymbol_t::value + tmp;

    return true;
  }

} // namespace libled

#endif
