//
//
//

#include "font.hpp"

namespace led_d
{
  const matrix_t& font_t::get (char s) const
  {
    static matrix_t unknown_symbol{0xFF, 0xFF, 0xFF};

    auto iter = m_map.find (s);
    if (iter == m_map.cend ())
      return unknown_symbol;

    return iter->second;
  }

  void font_t::add (char s, const matrix_t &matrix)
  {
    m_map.emplace (s, matrix);
  }
} // namespace led_d
