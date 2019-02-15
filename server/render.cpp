//
//
//
#include "font.hpp"
#include "log-wrapper.hpp"
#include "render.hpp"

namespace led_d
{
  render_t::render_t (const std::string &default_font)
    : m_font_dir (default_font)
  {
  }

  // bool render_t::init (const arg_t &arg)
  // {
  //   return m_font_dir.init (arg.default_font);
  // }

  bool render_t::pixelize (matrix_t &matrix, const std::string &text,
                           const std::string &format_string) const
  {
    format_t default_format;

    typedef format_t::list_t format_list_t;
    format_list_t format_list;
    if (format_t::split (format_list, format_string) == false)
      return pixelize (matrix, text, default_format);

    for (format_list_t::const_iterator iter = format_list.begin ();
         iter != format_list.end (); ++iter) {
      if (pixelize (matrix, text, *(*iter)) == false) {
        log_t::buffer_t msg;
        msg << "Failed to render formatted text " << text;
        log_t::error (msg);
      }
    }

    return true;
  }

  bool render_t::pixelize (matrix_t &matrix,
                           const std::string &text,
                           const format_t &format) const
  {
    auto font = m_font_dir.get_font (format.get_font ());

    std::size_t info_start (format.get_start ());
    std::size_t info_finish (info_start);
    info_finish += (format.is_unlimited () == true)
      ? text.size () : format.get_size ();

    matrix_t tmp_matrix;
    for (std::size_t pos = info_start; pos < info_finish; ++pos) {
      auto &symbol = font.get (text[pos]);
      tmp_matrix.insert (tmp_matrix.end (), symbol.begin (), symbol.end ());
    }

    if (format.get_inversion () == true)
      matrix_invert (tmp_matrix);

    matrix.insert (matrix.end (), tmp_matrix.begin (), tmp_matrix.end ());

    return true;
  }

} // namespace led_d
