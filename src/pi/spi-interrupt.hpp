/*
 * Handle spi interrupt line
 */
#ifndef SPI_INTERRUPT_HPP
#define SPI_INTERRUPT_HPP

#include <gpiod.h>

#include <functional>
#include <condition_variable>
#include <mutex>

#include "asio.hpp"

#include "util/condition-queue.hpp"

#include "spi-enable.hpp"

namespace led_d
{

  class spi_interrupt_t
  {
  public:

    using queue_t = util::condition_queue_t
      <bool,
       std::reference_wrapper<std::mutex>,
       std::reference_wrapper<std::condition_variable>>;

    // static constexpr char interrupt_rised = 'r';
    // static constexpr char interrupt_cleared = 'c';

    spi_interrupt_t (spi_enable_t &spi_enable,
                     queue_t &interrupt_queue, asio::io_context &context);
    ~spi_interrupt_t ();

    void start ();
    void stop ();

  private:

    // handle fd event
    void handle_event (const asio::error_code &errc);

    spi_enable_t &m_spi_enable;

    gpiod_line *m_interrupt;

    queue_t &m_queue;
    asio::posix::stream_descriptor m_descriptor;
  };

} // namespace led_d

#endif
