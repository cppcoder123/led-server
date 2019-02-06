//
//
//
#ifndef UNIX_TOKEN_HPP
#define UNIX_TOKEN_HPP

#include <string>
#include <utility>
#include <vector>

#include "patch.hpp"

namespace unix
{

  class token_t
  {

  public:

    typedef std::pair<std::size_t/*start pos*/,
                      std::size_t/*pos finish*/> position_pair_t;
    typedef std::vector<position_pair_t> pair_vector_t;

    
    static bool tokenize (pair_vector_t &dst, const std::string &src,
                          char refsymbol, bool first_only = false);

    // decoding
    template <typename info_t, typename ...pack_t>
    static bool convert (const std::string &src,
                         const pair_vector_t &pair_vector,
                         info_t &info, pack_t &...pack);

    // encoding
    template <typename info_t, typename ...pack_t>
    static void convert (std::string &dst,
                         char refsymbol,
                         const info_t &info, pack_t &...pack);
    
    template <typename dst_t>   // decode from string
    static bool decode (dst_t &dst, const std::string &src);

    template <typename src_t>   // encode to string
    static std::string encode (const src_t &src);

  private:

    template <typename info_t, typename... pack_t>
    static bool convert_info (const std::string &src,
                              const pair_vector_t &pair_vector,
                              std::size_t vector_index,
                              info_t &info, pack_t &...pack);

    static bool convert_info (const std::string &src,
                              const pair_vector_t &pair_vector,
                              std::size_t vector_index);

    static void convert (std::string &dst,
                         char refsymbol);

  };

  inline bool token_t::tokenize (pair_vector_t &dst, const std::string &src,
                                 char refsymbol, bool first_only)
  {
    std::size_t pos = 0, prev_pos = src.find_first_not_of (refsymbol);
    if (prev_pos == std::string::npos)
      return false;
    
    do {
      pos = src.find_first_of (refsymbol, prev_pos);
      std::size_t size = (pos != std::string::npos)
        ? pos - prev_pos : src.size () - prev_pos;
      dst.push_back (position_pair_t (prev_pos, size));
      if (first_only == true)
        return true;
      //
      if ((pos == std::string::npos)
          || (++pos >= src.size ()))
        return true;
      prev_pos = src.find_first_not_of (refsymbol, pos);
    } while (prev_pos != std::string::npos);

    return true;
  }  

  template <typename info_t, typename... pack_t>
  bool token_t::convert (const std::string &src,
                         const pair_vector_t &pair_vector,
                         info_t &info, pack_t &...pack)
  {
    return convert_info (src, pair_vector, 0, info, pack...);
  }

  template <typename info_t, typename... pack_t>
  void token_t::convert (std::string &dst,
                         char refsymbol,
                         const info_t &info, pack_t &...pack)
  {
    dst += encode (info) + refsymbol;
    return convert (dst, refsymbol, pack...);
  }

  inline void token_t::convert (std::string &dst,
                                char refsymbol)
  {
    // recursion terminator
  }

  template <typename info_t>
  bool token_t::decode (info_t &dst, const std::string &src)
  {
    std::istringstream stream (src);
    return (stream >> dst) ? true : false;
  }

  template <>
  inline bool token_t::decode<std::string>(std::string &dst,
                                           const std::string &src)
  {
    dst = src;
    return true;
  }

  template <typename info_t>
  std::string token_t::encode (const info_t &src)
  {
    // std::ostringstream stream;
    // stream << src;
    // return stream.str ();// std::to_string (src);
    return patch::to_string (src);
  }

  template <>
  inline std::string token_t::encode<std::string>(const std::string &src)
  {
    return src;
  }

  template <typename info_t, typename... pack_t>
  bool token_t::convert_info (const std::string &src,
                              const pair_vector_t &pair_vector,
                              std::size_t vector_index,
                              info_t &info, pack_t &...pack)
  {
    position_pair_t pair (pair_vector[vector_index]);
    if (decode (info, src.substr (pair.first, pair.second)) == false)
      return false;

    return (convert_info (src, pair_vector, ++vector_index, pack...) == true)
      ? true : false;
  }

  inline bool token_t::convert_info (const std::string &src,
                                     const pair_vector_t &pair_vector,
                                     std::size_t vector_index)
  {
    // termination
    return true;
  }
  
} // namespace unix

#endif
