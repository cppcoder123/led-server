//
// Should we continue writing to device ()?
//
#ifndef LED_D_BLOCK_HPP
#define LED_D_BLOCK_HPP

#include "device-codec.hpp"

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

    using char_t = core::device::codec_t::char_t;

    bool can_go () const;

    void tighten (char_t id);   // error if already tightened with other id
    void relax (char_t id);     // error here if id mismatch

  private:
    bool m_pending;
    char_t m_pending_id;
  };

} // namespace led_d

#endif
