//
// Should we continue writing to device ()?
//
#ifndef MCU_BLOCK_HPP
#define MCU_BLOCK_HPP

#include <cstdint>

namespace led_d
{

  class mcu_block_t
  {
  public:
    mcu_block_t ()
      : m_pending (false),
        m_pending_id (0)
    {
    }
    ~mcu_block_t () = default;

    bool is_engaged () const;

    void engage (uint8_t id);   // error if already tightened with other id
    void relax (uint8_t id);     // error here if id mismatch

  private:
    // std::mutex m_mutex;

    bool m_pending;
    uint8_t m_pending_id;
  };

} // namespace led_d

#endif
