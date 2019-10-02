//
//
//

#include "unix/log.hpp"

#include "daemon.hpp"
#include "network.hpp"

namespace led_d
{

  daemon_t::daemon_t (const arg_t &arg)
    : m_handle (arg.default_font),
      m_network (arg.port, m_io_context, m_handle.network_queue ()),
      m_mcu (arg.device, m_handle.from_mcu_queue (), arg.spi_msg),
      m_gpio (m_mcu.gpio_queue (), m_io_context)
  {
    m_handle.to_mcu_queue (m_mcu.to_mcu_queue ());
  }

  daemon_t::~daemon_t ()
  {
  }

  bool daemon_t::start ()
  {
    try {
      m_network.start ();
      m_handle_thread = std::thread (&handle_t::start, &m_handle);
      m_mcu_thread = std::thread (&mcu_t::start, &m_mcu);
    }
    catch (std::exception &e) {
      log_t::error (std::string ("Daemon: ") + e.what ());
      return false;
    }

    return true;
  }

  void daemon_t::stop ()
  {
    m_network.stop ();
    m_handle.stop ();
    m_mcu.stop ();
    m_gpio.stop ();

    m_handle_thread.join ();
    m_mcu_thread.join ();
  }

} // namespace led_d
