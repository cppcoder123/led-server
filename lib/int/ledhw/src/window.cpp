//
//
//
#include "window.hpp"

namespace ledhw
{
  const window_t::column_t window_t::m_empty_column;
  
  window_t::window_t (const libled::matrix_t &matrix)
    : m_position (0),
      m_matrix (matrix)
  {
  }
  window_t::window_t (const libled::matrix_t &matrix,
                      std::size_t start_position)
    : m_position (-static_cast<int>(start_position)),
      m_matrix (matrix)
  {
  }
  
  const window_t::column_t& window_t::get_column (std::size_t position)
  {
    int index = m_position + static_cast<int>(position);
    
    if ((index < 0)
        || (index >= static_cast<int>(m_matrix.size ())))
      return m_empty_column;

    return m_matrix.get_column (static_cast<std::size_t>(index));
  }

  void window_t::left_shift ()
  {
    ++m_position;
  }
} // namespace ledhw
