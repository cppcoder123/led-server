//
//
//

#ifndef INFO_HPP
#define INFO_HPP

#include <optional>
#include <string>

namespace led_d
{
  class info_t
  {

  public:

    info_t (const std::string &src_text, const std::string &src_format);
    info_t (const std::string &src_text,
            const std::string &src_format, uint8_t src_flag);
    ~info_t () {}

    const std::string text;
    const std::string format;
    const std::optional<uint8_t> flag;
  };

  inline info_t::info_t (const std::string &src_text,
                         const std::string &src_format)
    : text (src_text),
      format (src_format)
  {
  }

  inline info_t::info_t (const std::string &src_text,
                         const std::string &src_format, uint8_t src_flag)
    : text (src_text),
      format (src_format),
      flag (src_flag)
  {
  }
      
}

#endif
