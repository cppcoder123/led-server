//
//
//
#ifndef LED_D_FONT_HPP
#define LED_D_FONT_HPP

#include <limits>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "matrix.hpp"

namespace led_d
{

  class font_t : public libled::matrix_t
  {

  public:

    font_t ();
    virtual ~font_t () {};

    typedef std::pair<std::size_t/*symbol start*/,
                      std::size_t/*symbol size*/> symbol_t;

    symbol_t get_symbol (char s) const;
    void add_symbol (char s, const matrix_t &matrix);

  private:

    static std::size_t get_symbol_index (char s);
    
    typedef std::vector<symbol_t> vector_t;
    vector_t m_vector;

    // char_min can be zero or less, so
    static const int shift = -std::numeric_limits<char>::min ();
    static const std::size_t max_index
    = static_cast<std::size_t>(std::numeric_limits<char>::max ()
                               - std::numeric_limits<char>::min () + 1);
  };
  
} // namespace led_d

#endif
