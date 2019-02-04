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
#include "message-ptr.hpp"
#include "network.hpp"
#include "handle.hpp"
#include "spi.hpp"
#include "type-def.hpp"

namespace led_d
{

  class daemon_t
  {

  public:

    daemon_t ();
    daemon_t (const daemon_t &arg) = delete;
    ~daemon_t ();
    
    int start (const arg_t &arg);
    void stop ();

  private:

    // fixme: add separate 'notify' function for queues
    // to be able to wait on it
    message_queue_t m_network_queue; // from network
    msg_queue_t m_spi_queue;         // from spi

    asio::io_context m_asio_context;

    network_t m_network;
    content_t m_content;
    spi_t m_spi;
    handle_t m_handle;
    
    std::thread m_network_thread;
    std::thread m_handle_thread;
    std::thread m_spi_thread;
  };

} // namespace led_d

#endif
