//
//
//

#include "unix/log.hpp"

#include "font.hpp"

namespace led_d
{
  namespace {
    constexpr auto id_min = 0;
    constexpr auto id_max = 127;

    const matrix_t& empty_symbol ()
    {
      static const matrix_t s_empty;
      return s_empty;
    }

    matrix_t transpose (const matrix_t &src)
    {
      constexpr auto matrix_size = 8;
      if (src.size () != matrix_size)
        return src;

      matrix_t dst (matrix_size, 0xFF);

      uint8_t src_mask = (1 << 0);
      for (auto &dst_byte : dst) {
        uint8_t rotated = 0;
        for (auto src_iter = src.rbegin ();
             src_iter != src.rend (); ++src_iter) {
          rotated <<= 1;
          if (*src_iter & src_mask)
            rotated |= (1 << 0);
        }
        dst_byte = rotated;
        src_mask <<= 1;
      }

      return dst;
    }

  }

  font_t::font_t ()
    : m_data (fill_data ())
  {
  }

  const matrix_t& font_t::get (char s) const
  {
    if (is_basic (s) == true)
      return find_symbol (to_uint16 (s));

    if (m_first_byte == 0) {
      m_first_byte = s;
      return empty_symbol ();
    }

    // log_t::buffer_t buf;
    // buf << "font: 2-byte symbol"
    //     << std::hex << (unsigned) m_first_byte
    //     << " - "
    //     << std::hex << (unsigned) to_uint8 (s);
    // log_t::error (buf);

    // auto &symbol = find_symbol (to_uint16 (m_first_byte, to_uint8 (s)));
    auto &symbol = find_symbol (to_uint16 (to_uint8 (s), m_first_byte));
    // m_first_byte = (symbol.empty () == true) ? to_uint8 (s) : 0;
    if (symbol.empty () == true) {
      log_t::buffer_t buf;
      buf << "font: Failed to map 2-byte symbol"
          << std::hex << (unsigned) m_first_byte << " - "
          << std::hex << (unsigned) to_uint8 (s);
      log_t::error (buf);
      m_first_byte = 0;
      return find_symbol (to_uint16 ('*'));
    }
    m_first_byte = 0;

    return symbol;
  }

  bool font_t::is_basic (char s)
  {
    return ((s >= id_min) && (s <= id_max)) ? true : false;
  }

  uint8_t font_t::to_uint8 (char s)
  {
    return static_cast<uint8_t>(s);
  }

  uint16_t font_t::to_uint16 (char s)
  {
    return static_cast<uint16_t>(s);
  }

  uint16_t font_t::to_uint16 (uint8_t first, uint8_t second)
  {
    constexpr auto first_mask = 0x1F; // right 5 bits
    constexpr auto second_mask = 0x3F; // right 6 bits
    uint16_t buf = first & first_mask;
    buf <<= 6;
    return buf | (second & second_mask);
  }

  const matrix_t& font_t::find_symbol (uint16_t key, const map_t &symbol_map)
  {
    auto iter = symbol_map.find (key);
    if (iter == symbol_map.end ())
      return empty_symbol ();

    auto ptr = iter->second;

    return *ptr;
    
  }

  const matrix_t& font_t::find_symbol (uint16_t key) const
  {
    return find_symbol (key, m_data);
  }

  void font_t::add_basic_symbol (map_t &data,
                                 char key, const matrix_t &raw_symbol)
  {
    auto symbol = transpose (raw_symbol);
    auto ptr = std::make_shared<matrix_t>(symbol);
    data.insert (std::make_pair (to_uint16 (key), ptr));
  }

  void font_t::add_extended_symbol (map_t &data, uint8_t first, uint8_t second,
                                    const matrix_t &raw_symbol, bool rotate)
  {
    auto symbol = (rotate == true) ? transpose (raw_symbol) : empty_symbol ();
    auto ptr = std::make_shared<matrix_t>((rotate == true) ? symbol : raw_symbol);
    data.insert (std::make_pair (to_uint16 (first, second), ptr));
  }

  font_t::map_t font_t::fill_data ()
  {
    map_t data;

    auto add_basic = [&data] (char key, const matrix_t &symbol)
      {add_basic_symbol (data, key, symbol);};
    auto add_german = [&data] (uint8_t second, const matrix_t &symbol)
      {add_extended_symbol (data, 0xC3, second, symbol, true);};
    auto add_russian = [&data] (bool d0, uint8_t second, const matrix_t &symbol)
      {add_extended_symbol
       (data, (d0 == true) ? 0xD0 : 0xD1, second, symbol, true);};
    auto map_russian = [&data] (bool d0, uint8_t second, char basic)
      {
        auto &symbol = find_symbol (to_uint16 (basic), data);
        if (symbol.empty () == true)
          return;

        auto first = (d0 == true) ? 0xD0 : 0xD1;
        add_extended_symbol (data, first, second, symbol, false);
      };

    // **************************************************
    add_basic (' ', { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
    add_basic ('!', { 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00});
    add_basic ('"', { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
    add_basic ('#', { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00});
    add_basic ('$', { 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00});
    add_basic ('%', { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00});
    add_basic ('&', { 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00});
    add_basic ('\'', { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00});
    add_basic ('(', { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00});
    add_basic (')', { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00});
    add_basic ('*', { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00});
    add_basic ('+', { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00});
    add_basic (',', { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06});
    add_basic ('-', { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00});
    add_basic ('.', { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00});
    add_basic ('/', { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00});
    add_basic ('0', { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00});
    add_basic ('1', { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00});
    add_basic ('2', { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00});
    add_basic ('3', { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00});
    add_basic ('4', { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00});
    add_basic ('5', { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00});
    add_basic ('6', { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00});
    add_basic ('7', { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00});
    add_basic ('8', { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00});
    add_basic ('9', { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00});
    add_basic (':', { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00});
    add_basic ('/', { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06});
    add_basic ('<', { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00});
    add_basic ('=', { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00});
    add_basic ('>', { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00});
    add_basic ('?', { 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00});
    add_basic ('@', { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00});
    add_basic ('A', { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00});
    add_basic ('B', { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00});
    add_basic ('C', { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00});
    add_basic ('D', { 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00});
    add_basic ('E', { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00});
    add_basic ('F', { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00});
    add_basic ('G', { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00});
    add_basic ('H', { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00});
    add_basic ('I', { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00});
    add_basic ('J', { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00});
    add_basic ('K', { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00});
    add_basic ('L', { 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00});
    add_basic ('M', { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00});
    add_basic ('N', { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00});
    add_basic ('O', { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00});
    add_basic ('P', { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00});
    add_basic ('Q', { 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00});
    add_basic ('R', { 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00});
    add_basic ('S', { 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00});
    add_basic ('T', { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00});
    add_basic ('U', { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00});
    add_basic ('V', { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00});
    add_basic ('W', { 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00});
    add_basic ('X', { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00});
    add_basic ('Y', { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00});
    add_basic ('Z', { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00});
    add_basic ('[', { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00});
    add_basic ('\\', { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00});
    add_basic (']', { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00});
    add_basic ('^', { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00});
    add_basic ('_', { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF});
    add_basic ('`', { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00});
    add_basic ('a', { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00});
    add_basic ('b', { 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00});
    add_basic ('c', { 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00});
    add_basic ('d', { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00});
    add_basic ('e', { 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00});
    add_basic ('f', { 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00});
    add_basic ('g', { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F});
    add_basic ('h', { 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00});
    add_basic ('i', { 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00});
    add_basic ('j', { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E});
    add_basic ('k', { 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00});
    add_basic ('l', { 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00});
    add_basic ('m', { 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00});
    add_basic ('n', { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00});
    add_basic ('o', { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00});
    add_basic ('p', { 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F});
    add_basic ('q', { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78});
    add_basic ('r', { 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00});
    add_basic ('s', { 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00});
    add_basic ('t', { 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00});
    add_basic ('u', { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00});
    add_basic ('v', { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00});
    add_basic ('w', { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00});
    add_basic ('x', { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00});
    add_basic ('y', { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F});
    add_basic ('z', { 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00});
    add_basic ('{', { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00});
    add_basic ('|', { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00});
    add_basic ('}', { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00});
    add_basic ('~', { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
    // **************************************************
    add_german (0x84, // a-big-umlaut
                {0x22, 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33});
    add_german (0xA4, // a-small-umlaut
                {0x22, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00});
    add_german (0x96, // o-big-umlaut
                {0x22, 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C});
    add_german (0xB6, // o-small-umlaut
                {0x22, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00});
    add_german (0x9C, // u-big-umlaut
                {0x22, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F});
    add_german (0xBC, // u-small-umlaut
                {0x22, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00});
    // **************************************************
    map_russian (true, 0x90, 'A');
    add_russian (true, 0x91, {0x3F, 0x3F, 0x03, 0x1F, 0x23, 0x23, 0x1F, 0x00}); // Big BE

    map_russian (true, 0x95, 'E');

    map_russian (true, 0x9A, 'K');

    map_russian (true, 0x9C, 'M');
    map_russian (true, 0x9D, 'H');
    map_russian (true, 0x9E, 'O');

    map_russian (true, 0xA0, 'P');
    map_russian (true, 0xA1, 'C');
    map_russian (true, 0xA2, 'T');

    map_russian (true, 0xB0, 'a');
    // small be
    add_russian (true, 0xB1, {0x7E, 0x03, 0x06, 0x3E, 0x43, 0x43, 0x3F, 0x00});

    map_russian (true, 0xB5, 'e');

    map_russian (true, 0xB9, 'k');

    map_russian (true, 0xBE, 'o');

    map_russian (false, 0x80, 'p');
    map_russian (false, 0x81, 'c');

    // **************************************************

    // **************************************************
    
    return data;
  }

  
} // namespace led_d
