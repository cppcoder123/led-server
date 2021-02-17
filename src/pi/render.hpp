//
// Convert text to grafic
//
#ifndef LED_D_RENDER_HPP
#define LED_D_RENDER_HPP

#include <string>

#include "util/format.hpp"

#include "arg.hpp"
#include "font.hpp"
#include "matrix.hpp"

namespace led_d
{

  class render_t
  {

  public:

    render_t () = default;
    ~render_t () = default;

    bool pixelize (matrix_t &dst,
                   const std::string &text, const std::string &format) const;

  private:

    using format_t = util::format_t;
    bool pixelize (matrix_t &matrix,
                   const std::string &text, const format_t &format) const;

    font_t m_font;
  };

} // namespace led_d

#endif
