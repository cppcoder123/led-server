//
//
//

#include "idle-request.hpp"

namespace led_d
{

  idle_request_t::idle_request_t ()
  {
    action = request_t::insert;
    info = "Nothing to display";
  }

} // namespace led_d
