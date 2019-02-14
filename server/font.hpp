//
//
//
#ifndef FONT_HPP
#define FONT_HPP

#include <unordered_map>

#include "matrix.hpp"

namespace led_d
{

  class font_t
  {

  public:

    font_t () {};
    ~font_t () {};

    const matrix_t& get (char s) const;
    void add (char s, const matrix_t &symbol);

  private:

    using map_t = std::unordered_map<char, matrix_t>;
    map_t m_map;
  };

} // namespace led_d

#endif
