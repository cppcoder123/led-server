//
//
//
#ifndef UNIX_CODEC_HPP
#define UNIX_CODEC_HPP

#include <sstream>
#include <string>
#include <vector>

#include "patch.hpp"
#include "token.hpp"


namespace unix
{
  template <typename refsymbol_t>
  class codec_t
  {

  public:

    static const std::size_t max_size = 1024;
    static const std::size_t header_size = 4;

    // decode length
    static bool decode (const std::string &src, std::size_t &msg_size);

    // decode body
    template <typename info_t>
    static bool decode (const std::string &src, info_t &dst);

    template <typename info_t>
    static bool encode (const info_t &src, std::string &dst);

  };

  //
  //
  //

  template <typename refsymbol_t>
  bool codec_t<refsymbol_t>::
  decode (const std::string &src, std::size_t &msg_size)
  {
    token_t::pair_vector_t pair_vector;
    if (token_t::tokenize (pair_vector, src, refsymbol_t::value, true) == false)
      return false;
    return token_t::convert (src, pair_vector, msg_size);
  }

  template <typename refsymbol_t> template <typename info_t>
  bool codec_t<refsymbol_t>::
  decode (const std::string &src, info_t &dst)
  {
    token_t::pair_vector_t pair_vector;
    if (token_t::tokenize (pair_vector, src, refsymbol_t::value) == false)
      return false;
    std::string tmp = src.substr (header_size);
    return dst.decode (src, pair_vector);
  }

  template <typename refsymbol_t> template <typename info_t>
  bool codec_t<refsymbol_t>::
  encode (const info_t &src, std::string &dst)
  {
    std::string tmp = src.encode (refsymbol_t::value);
    std::size_t len = tmp.size () + header_size; // 3 size + 1 refsymbol

    if (len > max_size)
      return false;

    std::string prefix ((len < 10) ? "00" : (len < 100) ? "0" : "");
    dst = prefix + patch::to_string (len) + refsymbol_t::value + tmp;

    return true;
  }

} // namespace unix

#endif
