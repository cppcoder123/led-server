//
//
//

#include "unix/constant.h"
#include "unix/log.hpp"

#include "mcu-block.hpp"

namespace led_d
{
  bool mcu_block_t::is_engaged () const
  {
    // std::lock_guard<std::mutex> guard (m_mutex);

    return m_pending;
  }

  void mcu_block_t::engage (unix::char_t id)
  {
    if (id == SERIAL_ID_TO_IGNORE) {
      log_t::buffer_t buf;
      buf << "mcu-block: Found avr serial id";
      log_t::error (buf);
      return;
    }

    if ((m_pending == true)
        && (m_pending_id != id)) {
      log_t::buffer_t buf;
      buf << "mcu-block: Trying to tighten with new id \"" << (int) id
          << "\", while old id \"" << (int) m_pending_id << "\" is not relaxed.";
      log_t::error (buf);
      return;
    }

    m_pending = true;
    m_pending_id = id;
  }

  void mcu_block_t::relax (unix::char_t id)
  {
    if ((id == SERIAL_ID_TO_IGNORE)
        || (m_pending == false))
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

    m_pending = false;
    m_pending_id = 0;
  }
} // namespace led_d

