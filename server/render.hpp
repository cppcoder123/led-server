//
// Convert text to grafic
//
#ifndef LED_D_RENDER_HPP
#define LED_D_RENDER_HPP

#include <string>

#include "unix/format.hpp"
#include "unix/matrix.hpp"

#include "arg.hpp"
#include "font-dir.hpp"

namespace led_d
{

  class render_t
  {

  public:

    render_t (const std::string &default_font);
    ~render_t () {}

    //bool init (const arg_t &arg);

    using matrix_t = unix::matrix_t;
    bool pixelize (matrix_t &dst,
                   const std::string &text, const std::string &format) const;

  private:

    using format_t = unix::format_t;
    bool pixelize (matrix_t &matrix,
                   const std::string &text, const format_t &format) const;

    font_dir_t m_font_dir;
  };

} // namespace led_d

#endif
