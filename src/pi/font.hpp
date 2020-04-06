//
//
//
#ifndef FONT_HPP
#define FONT_HPP

#include <map>
#include <memory>
#include <cstdint>
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

  private:

    static bool is_basic (char s);

    static uint8_t to_uint8 (char s);
    static uint16_t to_uint16 (char s);
    static uint16_t to_uint16 (uint8_t first, uint8_t second);

    const matrix_t& find_symbol (uint16_t key) const;

    using matrix_ptr_t = std::shared_ptr<matrix_t>;
    using map_t = std::map<unsigned, matrix_ptr_t>;

    static void basic_symbol_add (map_t &data,
                                  char key, const matrix_t &symbol);
    static void extended_symbol_add (map_t &data, uint8_t first,
                                     uint8_t second, const matrix_t &symbol);

    static map_t fill_data ();

    // handle only 2 bytes from utf-8, umlauts & russian
    mutable uint8_t m_first_byte;


    map_t m_data;
  };

} // namespace led_d

#endif
