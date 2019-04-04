//
//
//

#include "unix/log.hpp"

#include "daemon.hpp"
#include "network.hpp"

namespace led_d
{

  daemon_t::daemon_t (const arg_t &arg)
    : m_network (arg.port, m_asio_context, m_network_queue),
      m_handle (arg.default_font, m_network_queue,
                m_to_device_queue, m_from_device_queue),
      m_device (arg.device, m_to_device_queue,
                m_from_device_queue, arg.spi_msg)
  {
  }

  daemon_t::~daemon_t ()
  {
  }

  bool daemon_t::start ()
  {
    try {
      m_network.start ();
      m_handle_thread = std::thread (&handle_t::start, &m_handle);
      m_device_thread = std::thread (&device_t::start, &m_device);
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
    m_device.stop ();

    m_handle_thread.join ();
    m_device_thread.join ();
  }

} // namespace led_d
