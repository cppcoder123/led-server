/*
 *
 */
#ifndef SPI_IRQ_HPP
#define SPI_IRQ_HPP

#include <gpiod.h>

#include <functional>
#include <condition_variable>
#include <mutex>

#include "asio.hpp"

#include "unix/condition-queue.hpp"

namespace led_d
{

  class spi_irq_t
  {
  public:

    using queue_t = unix::condition_queue_t
      <char,
       std::reference_wrapper<std::mutex>,
       std::reference_wrapper<std::condition_variable>>;

    static constexpr char interrupt_rised = 'r';
    static constexpr char interrupt_cleared = 'c';

    spi_irq_t (queue_t &irq_queue, asio::io_context &context);
    ~spi_irq_t ();

  private:

    void start ();
    void stop ();

    // handle fd event
    void handle_event (const asio::error_code &errc);

    gpiod_chip *m_chip;

    gpiod_line *m_enable;
    gpiod_line *m_irq;

    queue_t &m_queue;
    asio::posix::stream_descriptor m_descriptor;
  };

} // namespace led_d

#endif
