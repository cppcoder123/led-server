//
//
//

#include "font.hpp"

namespace led_d
{
  namespace {
    static const matrix_t filler_a = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
    static const matrix_t filler_b = {0x0B, 0x0B, 0x0B, 0x0B, 0x0B};
  }

  font_t::font_t ()
    : m_vector {id_max + 1, filler_a}
  {
  }

  const matrix_t& font_t::get (char s) const
  {
    return (is_in_range (s) == true)
      ? m_vector[to_id (s)] : filler_b;
  }

  void font_t::add (char s, const matrix_t &matrix)
  {
    if (is_in_range (s) == false)
      return;

    m_vector[to_id (s)] = matrix;
  }

  bool font_t::is_in_range (char s)
  {
    return ((s >= id_min) && (s <= id_max)) ? true : false;
  }

  std::size_t font_t::to_id (char s)
  {
    return static_cast<std::size_t>(s);
  }
} // namespace led_d
