//
//
//

#include "unix/font-name.hpp"

#include "hex-font.hpp"
#include "font-dir.hpp"
#include "log-wrapper.hpp"

namespace led_d
{
  font_dir_t::font_dir_t ()
    : m_vector (font_name_t::id_size)
  {
    font_ptr_t ibm_font (std::make_shared<font_t>());
    if (hex::font_t::fill (*ibm_font, font_name_t::id_ibm) == true) {
      m_default_font = ibm_font;
      m_vector[font_name_t::id_ibm] = ibm_font;
    }

    font_ptr_t greek_font (std::make_shared<font_t>());
    if (hex::font_t::fill (*greek_font, font_name_t::id_greek) == true)
      m_vector[font_name_t::id_greek] = greek_font;

    font_ptr_t slim_font (std::make_shared<font_t>());
    if (hex::font_t::fill (*slim_font, font_name_t::id_slim) == true)
      m_vector[font_name_t::id_slim] = slim_font;
  }
  
  bool font_dir_t::init (const std::string &default_name)
  {
    if (default_name.empty () == true)
      return m_default_font != 0;

    font_name_t::id_t id (font_name_t::get (default_name));
    if (id == font_name_t::id_unknown) {
      log_t::buffer_t buf;
      buf << "Failed to find font \"" << default_name
          << "\", using hardcoded default";
      log_t::error (buf);
      return m_default_font != 0;
    }

    m_default_font = m_vector[id];
    return m_default_font != 0;
  }

  const font_dir_t::font_ptr_t font_dir_t::get_font (const std::string &name) const
  {
    font_name_t::id_t id (font_name_t::get (name));

    return (id != font_name_t::id_unknown)
      ? m_vector[id] : m_default_font;
  }

} // namespace led_d
