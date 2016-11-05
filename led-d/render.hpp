//
// Convert text to grafic
//
#ifndef LED_D_RENDER_HPP
#define LED_D_RENDER_HPP

#include <string>

#include "libled/format.hpp"
#include "libled/matrix.hpp"

#include "arg.hpp"
#include "font-dir.hpp"

namespace led_d
{

  class render_t
  {

  public:

    render_t () {}
    ~render_t () {}

    bool init (const arg_t &arg);
    
    typedef libled::matrix_t matrix_t;
    bool pixelize (matrix_t &dst,
                   const std::string &text, const std::string &format) const;

    

  private:

    typedef libled::format_t format_t;
    bool pixelize (matrix_t &matrix,
                   const std::string &text, const format_t &format) const;

    font_dir_t m_font_dir;
  };
  
} // namespace led_d

#endif
