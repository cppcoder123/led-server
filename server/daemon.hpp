//
//
//
#ifndef LED_D_DAEMON_HPP
#define LED_D_DAEMON_HPP

#include <memory>
#include <thread>

#include "asio.hpp"

#include "arg.hpp"
#include "content.hpp"
#include "network.hpp"
#include "handle.hpp"
#include "spi.hpp"
#include "type-def.hpp"

namespace led_d
{

  class daemon_t
  {

  public:

    daemon_t (const arg_t &arg);
    daemon_t (const daemon_t &arg) = delete;
    ~daemon_t ();

    bool start ();
    void stop ();

    asio::io_context& get_context () {return m_asio_context;}

  private:

    unix_queue_t m_network_queue; // from network
    mcu_queue_t m_from_spi_queue;
    mcu_queue_t m_to_spi_queue;

    asio::io_context m_asio_context;

    network_t m_network;
    content_t m_content;
    handle_t m_handle;
    spi_t m_spi;

    //std::thread m_network_thread;
    std::thread m_handle_thread;
    std::thread m_spi_thread;
  };

} // namespace led_d

#endif
