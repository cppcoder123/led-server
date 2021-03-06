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
#include "initial.hpp"
#include "message-queue.hpp"
#include "message-ptr.hpp"
#include "serial.hpp"

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

    void network_load (const arg_t &arg);
    void update_load ();

    asio::io_service m_asio_service;
    std::unique_ptr <serial_t> m_serial;
    std::thread m_network_thread;

    content_t m_content;
    std::thread m_content_thread;

    std::thread m_update_thread;
    bool m_update_go;

    std::thread m_initial_thread;
    initial_t m_initial;
    
    queue_t m_message_queue;
  };
  
} // namespace led_d

#endif
