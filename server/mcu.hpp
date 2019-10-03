/**
 *
 */
#ifndef MCU_HPP
#define MCU_HPP

#include <stdint.h>

#include <condition_variable>
#include <mutex>
#include <string>

#include "block.hpp"
#include "handle.hpp"
#include "mcu-queue.hpp"
#include "network-queue.hpp"
#include "parse.hpp"
#include "spi-dev.hpp"
#include "spi-irq.hpp"

namespace led_d
{

  class mcu_t
  {

  public:

    using irq_queue_t = spi_irq_t::queue_t;

    mcu_t (const std::string &path,
           mcu_queue_t &from_queue, bool show_msg);
    mcu_t (const mcu_t&) = delete;
    ~mcu_t ();

    void start ();
    void stop ();

    mcu_queue_t& to_mcu_queue () {return m_to_queue;}
    irq_queue_t& irq_queue () {return m_irq_queue;}

  private:

    void write_msg (const mcu_msg_t &msg);

    bool m_go;

    std::mutex m_mutex;
    std::condition_variable m_condition;
    mcu_queue_t m_to_queue;     // to spi
    mcu_queue_t &m_from_queue;  // from spi

    irq_queue_t m_irq_queue;    // from spi-irq
    bool m_interrupt_rised;

    spi_dev_t m_channel;

    block_t m_block;
    parse_t m_parse;

    bool m_show_msg;
  };

} // namespace led_d

#endif
