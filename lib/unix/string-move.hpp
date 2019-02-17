//
//
//
#ifndef STRING_MOVE_HPP
#define STRING_MOVE_HPP

#include <algorithm>
#include <array>
#include <string>

namespace unix
{
  template <typename array_t>
  std::size_t string_move (const std::string &src, array_t &dst_1, std::string &dst_2);

  template <typename array_t>
  std::size_t string_move (std::string &src, array_t &array);

  template <typename array_t>
  void string_move_append (array_t &src, std::size_t src_len, std::string &dst);

  //
  // --------------------------------------------------
  //

  template <typename array_t>
  std::size_t string_move (const std::string &src, array_t &dst_1, std::string &dst_2)
  {
    // 1. src <= dst
    if (src.size () <= dst_1.size ()) {
      std::copy (src.begin (), src.end (), dst_1.begin ());
      return src.size ();
    }

    // 2. src > dst
    std::copy_n (src.begin (), dst_1.size (), dst_1.begin ());
    dst_2 = src.substr (dst_1.size ());

    return dst_1.size ();
  }

  template <typename array_t>
  std::size_t string_move (std::string &src, array_t &dst)
  {
    if (src.size () <= dst.size ()) {
      std::copy (src.begin (), src.end (), dst.begin ());
      auto len = src.size ();
      src.clear ();
      return len;
    }

    copy_n (src.begin (), dst.size (), dst.begin ());
    src.erase (0, dst.size ());
    return dst.size ();
  }

  template <typename array_t>
  void string_move_append (array_t &src, std::size_t src_len, std::string &dst)
  {
    for (std::size_t i = 0; i < src_len; ++i)
      dst += src[i];
    //std::copy_n (src.begin (), src_len, dst.begin ());

    // Does it make sense to clean array?
    // fill it, to simplify possible debug
    std::fill (src.begin (), src.end (), '?');
  }

} // namespace unix

#endif
