//
//
//

#include "mcu/constant.h"

#include "block.hpp"
#include "log-wrapper.hpp"

namespace led_d
{
  bool block_t::is_engaged () const
  {
    // std::lock_guard<std::mutex> guard (m_mutex);

    return m_pending;
  }
  
  void block_t::engage (char_t id)
  {
    if (id == SERIAL_ID_TO_IGNORE) {
      log_t::buffer_t buf;
      buf << "block: Found avr serial id";
      log_t::error (buf);
      return;
    }

    if ((m_pending == true)
        && (m_pending_id != id)) {
      log_t::buffer_t buf;
      buf << "block: Trying to tighten with new id \"" << (int) id
          << "\", while old id \"" << (int) m_pending_id << "\" is not relaxed.";
      log_t::error (buf);
      return;
    }

    m_pending = true;
    m_pending_id = id;
  }

  void block_t::relax (char_t id)
  {
    if ((id == SERIAL_ID_TO_IGNORE)
        || (m_pending == false))
      // not an error
      return;
    
    if (m_pending_id != id) {
      log_t::buffer_t buf;
      buf << "block: Trying to relax with wrong id \""
          << id << "\" while expecting \"" << m_pending_id << "\"";
      log_t::error (buf);
      return;
    }

    m_pending = false;
    m_pending_id = 0;
  }
} // namespace led_d

