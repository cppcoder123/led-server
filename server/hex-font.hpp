//
//
//
#ifndef LED_D_HEX_FONT_HPP
#define LED_D_HEX_FONT_HPP

#include "unix/font-name.hpp"

#include "font.hpp"


namespace led_d
{
  namespace hex
  {
    class font_t
    {
      
    public:

      typedef core::font_name_t font_name_t;
      typedef font_name_t::id_t id_t;

      static bool fill (led_d::font_t &font, id_t id);

    private:

      typedef core::matrix_t matrix_t;
      typedef matrix_t::column_t column_t;
      typedef column_t row_t;     // 8x8 font
      typedef std::vector<row_t> row_vector_t;
      typedef std::vector<row_vector_t> hex_font_t;
      //
      // slightly changed format
      typedef std::vector<column_t> column_vector_t;

      static const hex_font_t ibm_font;
      static const hex_font_t greek_font;
      static const column_vector_t slim_font;

      static bool fill (led_d::font_t &font,
                        const hex_font_t &hex_font, std::size_t shift);
      static bool fill (led_d::font_t &font,
                        const column_vector_t &hex_font, std::size_t shift);
      
    };

    
  }
}

#endif
