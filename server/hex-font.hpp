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

      using font_name_t = unix::font_name_t;
      using id_t = font_name_t::id_t;

      static bool fill (led_d::font_t &font, id_t id);

    private:

      using matrix_t = unix::matrix_t;
      using column_t = matrix_t::column_t;
      using row_t = column_t;   // 8x8 font
      using row_vector_t = std::vector<row_t>;
      using hex_font_t = std::vector<row_vector_t>;
      //
      // slightly changed format
      using column_vector_t = std::vector<column_t>;

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
