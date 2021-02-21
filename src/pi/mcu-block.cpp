//
//
//
#include <cstdint>

#include "const/constant.h"

#include "util/log.hpp"

#include "mcu-block.hpp"

namespace led_d
{
  mcu_block_t::mcu_block_t ()
    : m_pending_id (SERIAL_ID_TO_IGNORE)
  {
  }

  bool mcu_block_t::is_engaged () const
  {
    return !(m_pending_id == SERIAL_ID_TO_IGNORE);
  }

  void mcu_block_t::engage (uint8_t id)
  {
    if (id == SERIAL_ID_TO_IGNORE)
      return;

    if ((is_engaged () == true)
        && (m_pending_id != id)) {
      log_t::buffer_t buf;
      buf << "mcu-block: Trying to tighten with new id \"" << (int) id
          << "\", while old id \"" << (int) m_pending_id << "\" is not relaxed.";
      log_t::error (buf);
      return;
    }

    m_pending_id = id;
  }

  void mcu_block_t::relax (uint8_t id)
  {
    if ((id == SERIAL_ID_TO_IGNORE)
        || (is_engaged () == false))
      // not an error
      return;

    if (m_pending_id != id) {
      log_t::buffer_t buf;
      buf << "mcu-block: Trying to relax with wrong id \""
          << (int) id << "\" while expecting \""
          << (int) m_pending_id << "\"";
      log_t::error (buf);
      return;
    }

    m_pending_id = SERIAL_ID_TO_IGNORE;
  }
} // namespace led_d

