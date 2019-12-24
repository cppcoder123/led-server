/*
 *
 */
#include <limits>

#include "status.hpp"

namespace led_d
{

  status_t::status_t (command_id::value_t id, int value, const std::string &out)
    : m_id (id),
      m_value (value),
      m_out (out)
  {
  }

  int status_t::bad ()
  {
    return std::numeric_limits<int>::max ();
  }

  int status_t::timeout ()
  {
    return std::numeric_limits<int>::min ();
  }

} // led_d
