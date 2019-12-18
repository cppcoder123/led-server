/*
 *
 */

#include "command.hpp"

namespace led_d
{
  command_t::command_t (id_t id, std::string body)
    : m_primary_id (id),
      m_secondary_id (id),
      m_body (body)
  {
  }

  command_t::command_t (id_t primary_id, id_t secondary_id, std::string body)
    : m_primary_id (primary_id),
      m_secondary_id (secondary_id),
      m_body (body)
  {
  }
}
