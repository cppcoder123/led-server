//
//
//

#include "unix/format.hpp"

#include "content.hpp"

namespace led_d
{
  content_t::content_t ()
  {
    // fixme
  }

  content_t::~content_t ()
  {
    // fixme
  }

  void content_t::in (std::string info)
  {
    // fixme
  }

  content_t::out_info_t content_t::out ()
  {
    // fixme
    return std::make_pair ("no-info", unix::format_t::encode_empty ());
  }
} // led_d
