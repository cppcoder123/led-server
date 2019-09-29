//
//
//
#ifndef LED_D_DAEMON_HPP
#define LED_D_DAEMON_HPP

#include <memory>
#include <thread>

#include "asio.hpp"

#include "arg.hpp"
#include "device.hpp"
#include "network.hpp"
#include "handle.hpp"
#include "type-def.hpp"

namespace led_d
{

  class daemon_t
  {

  public:

    using mcu_queue_t = handle_t::mcu_queue_t;

    daemon_t (const arg_t &arg);
    daemon_t (const daemon_t &arg) = delete;
    ~daemon_t ();

    bool start ();
    void stop ();

    asio::io_context& get_context () {return m_asio_context;}

  private:

    unix_queue_t m_network_queue; // from network
    mcu_queue_t m_from_device_queue;
    mcu_queue_t m_to_device_queue;

    asio::io_context m_asio_context;

    network_t m_network;
    //content_t m_content;
    handle_t m_handle;
    device_t m_device;

    //std::thread m_network_thread;
    std::thread m_handle_thread;
    std::thread m_device_thread;
  };

} // namespace led_d

#endif
