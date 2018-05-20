//
// Not used
//
#ifndef LEDHW_WINDOW_HPP
#define LEDHW_WINDOW_HPP

#include <utility>              // std::size_t

#include "libled/matrix.hpp"

namespace ledhw
{
  class window_t
  {

  public:

    window_t () = delete;
    window_t (const libled::matrix_t &matrix);
    window_t (const libled::matrix_t &matrix, std::size_t start_position);
    ~window_t () {}

    typedef libled::matrix_t::column_t column_t;
    const column_t& get_column (std::size_t position);

    void left_shift ();

  private:

    int m_position;
    const libled::matrix_t &m_matrix;

    static const column_t m_empty_column;
    
  };
}

#endif
