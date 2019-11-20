//
//
//

#include "unix/log.hpp"

#include "base.hpp"
#include "network.hpp"

namespace led_d
{

  base_t::base_t (const arg_t &arg)
    : m_network_handle (arg.default_font),
      m_network (arg.port, m_io_context, m_network_handle.network_queue ()),
      m_mcu_handle (arg.device, m_network_handle.from_mcu_queue (), arg.spi_msg),
      m_irq (m_spi_open, m_mcu_handle.irq_queue (), m_io_context)
  {
    m_network_handle.to_mcu_queue (m_mcu_handle.to_mcu_queue ());
  }

  base_t::~base_t ()
  {
  }

  bool base_t::start ()
  {
    try {
      m_spi_open.start ();
      m_irq.start ();
      m_network.start ();
      m_network_thread = std::thread (&network_handle_t::start, &m_network_handle);
      m_mcu_thread = std::thread (&mcu_handle_t::start, &m_mcu_handle);
    }
    catch (std::exception &e) {
      log_t::error (std::string ("Base: ") + e.what ());
      return false;
    }

    return true;
  }

  void base_t::stop ()
  {
    m_spi_open.stop ();
    m_network.stop ();
    m_network_handle.stop ();
    m_mcu_handle.stop ();

    m_network_thread.join ();
    m_mcu_thread.join ();
  }

} // namespace led_d
