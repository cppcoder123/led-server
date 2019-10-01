/*
 *
 */

// #include <fcntl.h>
// #include <linux/types.h>
// #include <linux/spi/spidev.h>
// #include <sys/ioctl.h>
// #include <unistd.h>



#include <functional>
#include <thread>

#include "unix/log.hpp"
#include "mcu/constant.h"

#include "mcu.hpp"
#include "mcu-decode.hpp"
#include "mcu-encode.hpp"
#include "serial-id.hpp"

namespace led_d
{

  // namespace {
  //   auto block_delay = std::chrono::milliseconds (1);
  // } // namespace anonymous

  mcu_t::mcu_t (const std::string &/*path*/,
                mcu_queue_t &from_queue, bool show_msg)
    : //m_path (path),
      m_go (true),
      m_to_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_from_queue (from_queue),
      m_gpio_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_interrupt_rised (false),
      m_show_msg (show_msg)
  {
  }

  mcu_t::~mcu_t ()
  {
    m_spi.stop ();

    //m_gpio.stop ();
  }

  void mcu_t::start ()
  {
    // gpio is first, we need to enable level shifter
    //m_gpio.start ();

    // open unix device
    m_spi.start ();

    m_to_queue.push
      (mcu::encode::join (serial::get (), MSG_ID_VERSION, PROTOCOL_VERSION));

    while (m_go == true) {
      if (m_interrupt_rised == true)
        write_msg (mcu::encode::join (SERIAL_ID_TO_IGNORE, MSG_ID_QUERY));
      auto char_opt = m_gpio_queue.pop<false>();
      if (char_opt)
        m_interrupt_rised = (*char_opt == gpio_t::interrupt_rised)
          ? true : false;
      if (m_interrupt_rised == true)
        continue;
      {
        std::unique_lock lock (m_mutex);
        if ((m_gpio_queue.empty<false>() == true)
            && ((m_block.is_engaged () == true)
                || (m_to_queue.empty<false>() == true))) {
          m_condition.wait (lock);
          continue;
        }
      }

      auto msg = m_to_queue.pop<false> ();
      if (msg)
        write_msg (*msg);
    }
  }

  void mcu_t::stop ()
  {
    m_go = false;

    m_spi.stop ();

    // fixme: smth else ???
  }

  void mcu_t::write_msg (const mcu_msg_t &msg_src)
  {
    unix::char_t serial_id = mcu::decode::get_serial (msg_src);

    //
    // eye-catch | size | serial | msg-id | xxx
    //
    mcu_msg_t msg = msg_src;
    mcu::encode::wrap (msg);

    if (serial_id != SERIAL_ID_TO_IGNORE)
      m_block.engage (serial_id);

    mcu_msg_t in_msg;
    m_spi.transfer (msg, in_msg);

    if (m_show_msg == true) {
      log_t::buffer_t buf;
      if (serial_id != SERIAL_ID_TO_IGNORE) {
        buf << "serial out: " << (int) serial_id;
        log_t::info (buf);
      }
      log_t::clear (buf);
      buf << "out: ";
      for (auto &num : msg)
        buf << (int) num << " ";
      log_t::info (buf);
      log_t::clear (buf);
      buf << "in: ";
      for (auto &num : in_msg)
        buf << (int) num << " ";
      log_t::info (buf);
    }

    msg.clear ();
    for (auto &number : in_msg)
      if (m_parse.push (number, msg) == true) {
        unix::char_t serial = 0;
        unix::char_t msg_id = MSG_ID_EMPTY;
        if (mcu::decode::split (msg, serial, msg_id) == true) {
          m_block.relax (serial);
          if ((m_show_msg == true)
              && (serial != SERIAL_ID_TO_IGNORE)) {
            log_t::buffer_t buf;
            buf << "serial in: " << (int) serial;
            log_t::info (buf);
          }
          if (msg_id != MSG_ID_STATUS)
            // special handling is needed, otherwise just drop msg
            m_from_queue.push (msg);
        } else {
          log_t::buffer_t buf;
          buf << "spi: Failed to decode mcu message";
          log_t::error (buf);
        }
      }
  }

} // namespace led_d
