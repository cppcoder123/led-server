//
//
//

#include "font.hpp"

namespace led_d
{
  font_t::font_t ()
    : m_vector (max_index, symbol_t (0, 0))
  {
  }
  
  font_t::symbol_t font_t::get_symbol (char s) const
  {
    return m_vector[get_symbol_index (s)];
  }

  void font_t::add_symbol (char s, const matrix_t &matrix)
  {
    m_vector[get_symbol_index (s)]
      // base object call
      = symbol_t (core::matrix_t::size (), matrix.size ());
    add (matrix);
  }

  std::size_t font_t::get_symbol_index (char s)
  {
    return static_cast<std::size_t>(shift + static_cast<int>(s));
  }

} // namespace led_d
