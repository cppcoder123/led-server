/*
 *
 */

#include <cstdint>
#include <functional>
#include <thread>

#include "const/constant.h"

#include "util/log.hpp"

#include "mcu-decode.hpp"
#include "mcu-encode.hpp"
#include "mcu-handle.hpp"
#include "mcu-id.hpp"

namespace led_d
{

  mcu_handle_t::mcu_handle_t (mcu_queue_t &from_queue, bool show_msg)
    : m_go (true),
      m_to_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_from_queue (from_queue),
      m_interrupt_queue (std::ref (m_mutex), std::ref (m_condition)),
      m_interrupt_rised (false),
      m_show_msg (show_msg)
  {
  }

  mcu_handle_t::~mcu_handle_t ()
  {
    //m_device.stop ();
  }

  void mcu_handle_t::start ()
  {
    // open unix device
    m_device.start ();

    // m_to_queue.push
    //   (mcu::encode::join (mcu_id::get (), MSG_ID_VERSION, PROTOCOL_VERSION));

    while (m_go.load () == true) {
      // 1. if we have an interrupt, try to handle it first
      if (m_interrupt_rised == true)
        write_msg (mcu::encode::join (SERIAL_ID_TO_IGNORE, MSG_ID_IDLE));
      auto bool_opt = m_interrupt_queue.pop<false>();
      if (bool_opt)
        m_interrupt_rised = *bool_opt;
      if (m_interrupt_rised == true)
        continue;

      // 2. No interrupt => should we stop?
      //    Note: 'm_block' is handled from the same thread, so
      //          we don't need separate queue for it
      {
        std::unique_lock lock (m_mutex);
        if ((m_go.load () == true)
            && (m_interrupt_queue.empty<false>() == true)
            && ((m_block.is_engaged () == true)
                || (m_to_queue.empty<false>() == true))) {
          m_condition.wait (lock);
          continue;
        }
      }

      // 3. send smth only if not engaged
      if (m_block.is_engaged () == false) {
        auto msg = m_to_queue.pop<false> ();
        if (msg)
          write_msg (*msg);
      }
    }
  }

  void mcu_handle_t::stop ()
  {
    m_go.store (false);

    m_device.stop ();

    m_condition.notify_one ();
  }

  void mcu_handle_t::write_msg (const mcu_msg_t &msg_src)
  {
    uint8_t serial_id = 0;
    if (mcu::decode::get_serial (msg_src, serial_id) == false) {
      log_t::error ("mcu-handle: Failed to get serial id from outcoming msg");
      return;
    }

    //
    // eye-catch | size | serial | msg-id | xxx
    //
    mcu_msg_t msg = msg_src;
    mcu::encode::wrap (msg);

    // if (serial_id != SERIAL_ID_TO_IGNORE)
    m_block.engage (serial_id);
      
    mcu_msg_t in_msg;
    m_device.transfer (msg, in_msg);

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
    for (auto &number : in_msg) {
      if (m_mcu_parse.push (number, msg) == false)
        continue;
      uint8_t serial = SERIAL_ID_TO_IGNORE;
      if (mcu::decode::get_serial (msg, serial) == false) {
        log_t::error ("mcu-handle: Failed to decode mcu message");
        continue;
      }
      m_block.relax (serial);
      if ((m_show_msg == true)
          /*&& (serial != SERIAL_ID_TO_IGNORE)*/) {
        log_t::buffer_t buf;
        buf << "serial in: " << (int) serial;
        log_t::info (buf);
      }
      m_from_queue.push (msg);
    }
  }

} // namespace led_d
