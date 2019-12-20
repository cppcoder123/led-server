//
//
//

#include "unix/log.hpp"

#include "base.hpp"

namespace led_d
{

  base_t::base_t (const arg_t &arg)
    : m_handle (arg.default_font, arg.subject_regexp_list),
      //m_bash_in (m_io_context, m_handle.bash_queue ()),
      m_bash_out (m_io_context, m_handle.status_queue ()),
      m_mcu_handle (m_handle.from_mcu_queue (), arg.spi_msg),
      m_irq (m_spi_open, m_mcu_handle.irq_queue (), m_io_context)
  {
    m_handle.to_mcu_queue (m_mcu_handle.to_mcu_queue ());
    m_handle.command_queue (m_bash_out.command_queue ());
  }

  base_t::~base_t ()
  {
  }

  bool base_t::start ()
  {
    try {
      m_spi_open.start ();
      m_irq.start ();
      //m_bash_in.start ();
      m_handle_thread = std::thread (&handle_t::start, &m_handle);
      m_bash_out_thread = std::thread (&bash_out_t::start, &m_bash_out);
      m_mcu_thread = std::thread (&mcu_handle_t::start, &m_mcu_handle);
    }
    catch (std::exception &e) {
      log_t::error (std::string ("Base: ") + e.what ());
      return false;
    }

    return true;
  }

  void base_t::stop ()
  {
    m_spi_open.stop ();
    //m_bash_in.stop ();
    m_handle.stop ();
    m_bash_out.stop ();
    m_mcu_handle.stop ();

    m_handle_thread.join ();
    m_bash_out_thread.join ();
    m_mcu_thread.join ();
  }

} // namespace led_d
