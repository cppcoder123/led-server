//
//
//

#include "daemon.hpp"
#include "log-wrapper.hpp"
#include "network.hpp"
#include "session.hpp"

namespace led_d
{

  daemon_t::daemon_t (const arg_t &arg)
    : m_network (arg.port, m_asio_context, m_network_queue),
      m_content (arg.default_font, m_asio_context, m_to_spi_queue),
      m_handle (m_network_queue, m_from_spi_queue, m_content),
      m_spi (arg.device, m_to_spi_queue, m_from_spi_queue)
  {
  }

  daemon_t::~daemon_t ()
  {
  }

  bool daemon_t::start ()
  {
    try {
      // perhaps 2 threads are enough ?
      // 1  for network and 1 for spi
      // with 2 threads we can avoid using mutexes in content module

      //m_network_thread = std::thread (&network_t::start, &m_network);
      m_network.start ();
      m_handle_thread = std::thread (&handle_t::start, &m_handle);
      m_spi_thread = std::thread (&spi_t::start, &m_spi);
      m_content.start ();
    }
    catch (std::exception &e) {
      log_t::error (std::string ("Daemon: ") + e.what ());
      return false;
    }

    return true;
  }

  void daemon_t::stop ()
  {
    m_content.stop ();
    m_network.stop ();
    m_handle.stop ();
    m_spi.stop ();

    //m_network_thread.join ();
    m_handle_thread.join ();
    m_spi_thread.join ();
  }

} // namespace led_d
