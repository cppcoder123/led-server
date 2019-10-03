//
//
//
#ifndef BASE_HPP
#define BASE_HPP

#include <thread>

#include "asio.hpp"

#include "arg.hpp"
#include "mcu-handle.hpp"
#include "network-handle.hpp"
#include "network.hpp"
#include "spi-irq.hpp"

namespace led_d
{

  class base_t
  {

  public:

    base_t (const arg_t &arg);
    base_t (const base_t &arg) = delete;
    ~base_t ();

    bool start ();
    void stop ();

    asio::io_context& get_context () {return m_io_context;}

  private:

    asio::io_context m_io_context;

    network_handle_t m_network_handle;
    network_t m_network;
    mcu_handle_t m_mcu_handle;
    spi_irq_t m_irq;

    std::thread m_network_thread;
    std::thread m_mcu_thread;
  };

} // namespace led_d

#endif
