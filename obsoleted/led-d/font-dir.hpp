//
// Font storage
//
#ifndef LED_D_FONT_DIR_HPP
#define LED_D_FONT_DIR_HPP

#include <map>
#include <memory>
#include <string>

#include "unix/font-name.hpp"

#include "font.hpp"

namespace led_d
{
  class font_dir_t
  {

  public:

    font_dir_t (const std::string &default_font);
    ~font_dir_t () {}

    const font_t& get_font (const std::string &name) const;
    const font_t& get_font () const {return m_default_font;}

  private:

    using font_name_t = unix::font_name_t;
    using map_t = std::map<font_name_t::id_t, font_t>;

    static font_t get_info (font_name_t::id_t id);
    
    const map_t m_font_map;
    const font_t& m_default_font;

  };

} // namespace led_d


#endif
