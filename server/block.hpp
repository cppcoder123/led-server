//
// Should we continue writing to device ()?
//
#ifndef LED_D_BLOCK_HPP
#define LED_D_BLOCK_HPP

#include "unix/char-type.hpp"

namespace led_d
{

  class block_t
  {
  public:
    block_t ()
      : m_pending (false),
        m_pending_id (0)
    {
    }
    ~block_t () = default;

    bool is_engaged () const;

    void engage (unix::char_t id);   // error if already tightened with other id
    void relax (unix::char_t id);     // error here if id mismatch

  private:
    // std::mutex m_mutex;

    bool m_pending;
    unix::char_t m_pending_id;
  };

} // namespace led_d

#endif
