//
//
//
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstdint>
#include <vector>
#include <utility>

namespace led_d
{
  static const auto matrix_column_size = 8;
  using matrix_column_t = uint8_t;
  using matrix_t = std::vector<uint8_t>;

  void matrix_invert (matrix_t &matrix);

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
