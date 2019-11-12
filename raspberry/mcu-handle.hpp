/**
 *
 */
#ifndef MCU_HANDLE_HPP
#define MCU_HANDLE_HPP

#include <stdint.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>

#include "mcu-block.hpp"
#include "mcu-parse.hpp"
#include "mcu-queue.hpp"
#include "network-handle.hpp"
#include "network-queue.hpp"
#include "spi-dev.hpp"
#include "spi-irq.hpp"

namespace led_d
{

  class mcu_handle_t
  {

  public:

    using irq_queue_t = spi_irq_t::queue_t;

    mcu_handle_t (const std::string &path,
           mcu_queue_t &from_queue, bool show_msg);
    mcu_handle_t (const mcu_handle_t&) = delete;
    ~mcu_handle_t ();

    void start ();
    void stop ();

    mcu_queue_t& to_mcu_queue () {return m_to_queue;}
    irq_queue_t& irq_queue () {return m_irq_queue;}

  private:

    void write_msg (const mcu_msg_t &msg);

    std::atomic_bool m_go;

    std::mutex m_mutex;
    std::condition_variable m_condition;
    mcu_queue_t m_to_queue;     // to spi
    mcu_queue_t &m_from_queue;  // from spi

    irq_queue_t m_irq_queue;    // from spi-irq
    bool m_interrupt_rised;

    spi_dev_t m_device;         // unix device

    mcu_block_t m_block;
    mcu_parse_t m_mcu_parse;

    bool m_show_msg;
  };

} // namespace led_d

#endif
