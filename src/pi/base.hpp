//
//
//
#ifndef BASE_HPP
#define BASE_HPP

#include <thread>

#include "asio.hpp"

#include "arg.hpp"
#include "bash.hpp"
#include "handle.hpp"
#include "mcu-handle.hpp"
#include "spi-enable.hpp"
#include "spi-interrupt.hpp"

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

    handle_t m_handle;
    bash_t m_bash;
    mcu_handle_t m_mcu_handle;
    spi_enable_t m_spi_enable;
    spi_interrupt_t m_interrupt;

    std::thread m_handle_thread;
    std::thread m_bash_thread;
    std::thread m_mcu_thread;
  };

} // namespace led_d

#endif
