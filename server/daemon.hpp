//
//
//
#ifndef LED_D_DAEMON_HPP
#define LED_D_DAEMON_HPP

#include <memory>
#include <thread>

#include "asio.hpp"

#include "arg.hpp"
#include "handle.hpp"
#include "mcu.hpp"
#include "network.hpp"
#include "network-queue.hpp"

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

    asio::io_context m_asio_context;

    handle_t m_handle;
    network_t m_network;
    mcu_t m_mcu;

    //std::thread m_network_thread;
    std::thread m_handle_thread;
    std::thread m_mcu_thread;
  };

} // namespace led_d

#endif
