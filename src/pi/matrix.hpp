//
//
//
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstdint>
#include <list>
#include <vector>

namespace led_d
{
  using matrix_t = std::list<uint8_t>;

  //
  // --------------------------------------------------
  //
  inline void matrix_invert (matrix_t &matrix)
  {
    for (auto &x : matrix)
      x = ~x;
  }

} // namespace unix

#endif
