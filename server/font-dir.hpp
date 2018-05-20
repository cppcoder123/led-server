//
// Font storage
//
#ifndef LED_D_FONT_DIR_HPP
#define LED_D_FONT_DIR_HPP

#include <memory>
#include <string>
#include <vector>

#include "font-name.hpp"

#include "font.hpp"

namespace led_d
{
  class font_dir_t
  {

  public:

    font_dir_t ();
    ~font_dir_t () {}

    typedef std::shared_ptr<font_t> font_ptr_t;

    bool init (const std::string &default_font_name);
    
    bool is_font (const std::string &name) const;
    const font_ptr_t get_font (const std::string &name) const;
    const font_ptr_t get_font () const; // default

  private:

    typedef std::vector<font_ptr_t> vector_t;
    vector_t m_vector;

    font_ptr_t m_default_font;

    typedef libled::font_name_t font_name_t;
    
  };

  //
  //
  //
  inline bool font_dir_t::is_font (const std::string &name) const
  {
    font_name_t::id_t id (font_name_t::get (name));
    
    return (id != font_name_t::id_unknown) ? true : false;
  }

  inline const font_dir_t::font_ptr_t font_dir_t::get_font () const
  {
    return m_default_font;
  }
 
} // namespace led_d


#endif
