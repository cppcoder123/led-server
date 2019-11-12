/*
 *
 */


#include <functional>
#include <stdexcept>

#include "unix/log.hpp"

#include "spi-irq.hpp"

namespace
{
  auto irq_offset = 27;         // fixme: check
} // namespace

namespace led_d
{
  spi_irq_t::spi_irq_t (spi_open_t &spi_open, queue_t &irq_queue, asio::io_context &context)
    : m_spi_open (spi_open),
      m_irq (NULL),
      m_queue (irq_queue),
      m_descriptor (context)
  {
  }

  spi_irq_t::~spi_irq_t ()
  {
    stop ();
  }
  
  void spi_irq_t::start ()
  {
    m_irq = gpiod_chip_get_line (m_spi_open.chip (), irq_offset);
    if (m_irq == NULL)
      throw std::runtime_error ("spi_irq: Failed to open irq line");

    if (gpiod_line_request_both_edges_events (m_irq, m_spi_open.consumer ()) != 0)
      throw std::runtime_error ("spi_irq: Failed to request irq events");

    auto fd = gpiod_line_event_get_fd (m_irq);
    if (fd < 0)
      std::runtime_error ("spi_irq: Failed to get event fd");

    asio::error_code errc;
    m_descriptor.assign (fd, errc);
    if (errc) {
      std::ostringstream buf;
      buf << "spi_irq: Error during creating stream-descriptor \""
          << errc.message () << "\"";
      std::runtime_error (buf.str ());
    }

    m_descriptor.async_wait
      (asio::posix::stream_descriptor::wait_read,
       std::bind (&spi_irq_t::handle_event, this, std::placeholders::_1));
  }

  void spi_irq_t::stop ()
  {
    if (m_irq) {
      gpiod_line_release (m_irq);
      m_irq = nullptr;
    }
  }

  void spi_irq_t::handle_event (const asio::error_code &errc)
  {
    if (errc) {
      log_t::buffer_t buf;
      buf << "spi_irq: Failed to handle asio event: \""
          << errc.message () << "\"";
      log_t::error (buf);
      return;
    }

    struct gpiod_line_event event;
    if (gpiod_line_event_read_fd (m_descriptor.native_handle (), &event) != 0) {
      log_t::error ("spi_irq: Failed to read file descriptor event");
      return;
    }

    m_queue.push ((event.event_type == GPIOD_LINE_EVENT_RISING_EDGE)
                  ? interrupt_rised : interrupt_cleared);

    m_descriptor.async_wait
      (asio::posix::stream_descriptor::wait_read,
       std::bind (&spi_irq_t::handle_event, this, std::placeholders::_1));
  }

} // namespace led_d
