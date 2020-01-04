//
//
//
#ifndef FONT_HPP
#define FONT_HPP

#include <vector>

#include "matrix.hpp"

namespace led_d
{

  class font_t
  {

  public:

    font_t ();
    ~font_t () {};

    const matrix_t& get (char s) const;
    void add (char s, const matrix_t &symbol);

  private:

    static bool is_in_range (char s);
    static std::size_t to_id (char s);

    using vector_t = std::vector<matrix_t>;

    vector_t m_vector;
  };

} // namespace led_d

#endif
