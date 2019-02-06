//
//
//

#include "idle-request.hpp"

namespace led_d
{

  idle_request_t::idle_request_t ()
  {
    action = request_t::action_insert;
    info = "Nothing to display";
  }

} // namespace led_d
