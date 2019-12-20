/*
 *
 */


#include <functional>
#include <stdexcept>

#include "unix/log.hpp"

#include "spi-interrupt.hpp"

namespace
{
  auto interrupt_offset = 27;         // fixme: check
} // namespace

namespace led_d
{
  spi_interrupt_t::spi_interrupt_t (spi_open_t &spi_open,
                        queue_t &interrupt_queue, asio::io_context &context)
    : m_spi_open (spi_open),
      m_interrupt (NULL),
      m_queue (interrupt_queue),
      m_descriptor (context)
  {
  }

  spi_interrupt_t::~spi_interrupt_t ()
  {
    stop ();
  }
  
  void spi_interrupt_t::start ()
  {
    m_interrupt = gpiod_chip_get_line (m_spi_open.chip (), interrupt_offset);
    if (m_interrupt == NULL)
      throw std::runtime_error ("spi_interrupt: Failed to open interrupt line");

    if (gpiod_line_request_both_edges_events (m_interrupt, m_spi_open.consumer ()) != 0)
      throw std::runtime_error ("spi_interrupt: Failed to request interrupt events");

    auto fd = gpiod_line_event_get_fd (m_interrupt);
    if (fd < 0)
      std::runtime_error ("spi_interrupt: Failed to get event fd");

    asio::error_code errc;
    m_descriptor.assign (fd, errc);
    if (errc) {
      std::ostringstream buf;
      buf << "spi_interrupt: Error during creating stream-descriptor \""
          << errc.message () << "\"";
      std::runtime_error (buf.str ());
    }

    m_descriptor.async_wait
      (asio::posix::stream_descriptor::wait_read,
       std::bind (&spi_interrupt_t::handle_event, this, std::placeholders::_1));
  }

  void spi_interrupt_t::stop ()
  {
    if (m_interrupt) {
      gpiod_line_release (m_interrupt);
      m_interrupt = nullptr;
    }
  }

  void spi_interrupt_t::handle_event (const asio::error_code &errc)
  {
    if (errc) {
      log_t::buffer_t buf;
      buf << "spi_interrupt: Failed to handle asio event: \""
          << errc.message () << "\"";
      log_t::error (buf);
      return;
    }

    struct gpiod_line_event event;
    if (gpiod_line_event_read_fd (m_descriptor.native_handle (), &event) != 0) {
      log_t::error ("spi_interrupt: Failed to read file descriptor event");
      return;
    }

    // {
    //   constexpr auto rise = "spi-interrupt: Rising edge";
    //   constexpr auto fall = "spi-interrupt: Falling edge";
    //   if (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE)
    //     log_t::info (rise);
    //   else if (event.event_type == GPIOD_LINE_EVENT_FALLING_EDGE)
    //     log_t::info (fall);
    // }

    m_queue.push ((event.event_type == GPIOD_LINE_EVENT_RISING_EDGE)
                  ? interrupt_rised : interrupt_cleared);

    m_descriptor.async_wait
      (asio::posix::stream_descriptor::wait_read,
       std::bind (&spi_interrupt_t::handle_event, this, std::placeholders::_1));
  }

} // namespace led_d
