//
//
//
#ifndef LED_D_DAEMON_HPP
#define LED_D_DAEMON_HPP

#include <thread>

#include "asio.hpp"

#include "arg.hpp"
#include "display.hpp"
#include "message-queue.hpp"
#include "message-ptr.hpp"

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

    //typedef std::unique_ptr<std::thread> thread_ptr_t;

    asio::io_service m_asio_service;
    std::thread m_network_thread;

    display_t m_display;
    std::thread m_display_thread;

    std::thread m_update_thread;
    bool m_update_go;

    queue_t m_message_queue;
  };
  
} // namespace led_d

#endif
