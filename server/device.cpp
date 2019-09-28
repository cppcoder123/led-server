/*
 *
 */

// #include <fcntl.h>
// #include <linux/types.h>
// #include <linux/spi/spidev.h>
// #include <sys/ioctl.h>
// #include <unistd.h>

#include <thread>

#include "unix/log.hpp"
#include "mcu/constant.h"

#include "device.hpp"
#include "mcu-decode.hpp"
#include "mcu-encode.hpp"
#include "serial-id.hpp"

namespace led_d
{

  namespace {
    auto block_delay = std::chrono::milliseconds (1);
  } // namespace anonymous

  device_t::device_t (const std::string &/*path*/, mcu_queue_t &to_queue,
                      mcu_queue_t &from_queue, bool show_msg)
    : //m_path (path),
      m_go (true),
      m_to_queue (to_queue),
      m_from_queue (from_queue),
      m_show_msg (show_msg)
  {
  }

  device_t::~device_t ()
  {
    m_spi.stop ();

    m_gpio.stop ();
  }

  void device_t::start ()
  {
    // gpio is first, we need to enable level shifter
    m_gpio.start ();

    // open unix device
    m_spi.start ();

    m_to_queue.push
      (mcu::encode::join (serial::get (), MSG_ID_VERSION, PROTOCOL_VERSION));

    // fixme:
    // It is better to avoid using delay here,
    // but unclear how to do it
    //
    // We need to add second template parameter for queue
    // as mutex type and then pass either std::mutex as before
    // or pass std::ref(std::mutex) to share it between 2 queues
    // (or other things)

    while (m_go == true) {
      if (m_gpio.is_irq_raised () == true) {
        write_msg (mcu::encode::join (SERIAL_ID_TO_IGNORE, MSG_ID_QUERY));
        continue;
      }
      if (m_block.is_engaged () == true) {
        std::this_thread::sleep_for (block_delay);
        continue;
      }
      auto msg = m_to_queue.pop ();
      if (msg)
        write_msg (*msg);
      else
        std::this_thread::sleep_for (block_delay);
    }
  }

  void device_t::stop ()
  {
    m_go = false;

    m_spi.stop ();

    // fixme: smth else ???
  }

  void device_t::write_msg (const mcu_msg_t &msg_src)
  {
    char_t serial_id = mcu::decode::get_serial (msg_src);

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
        char_t serial = 0;
        char_t msg_id = MSG_ID_EMPTY;
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
