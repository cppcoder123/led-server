//
//
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdexcept>

#include "msg-id.h"

#include "uart.hpp"

namespace render
{
  uart_t::uart_t (const std::string &linux_device)
    : m_linux_device (linux_device)
  {
    device_open ();
    //
    send_receive_check ({MSG_ID_SLAVE_HANDSHAKE});
    send_receive_check ({MSG_ID_SLAVE_SHIFT,
          MSG_ID_SHIFT_DELAY, 30, MSG_ID_SHIFT_PAUSE, 150});
    send_receive_check ({MSG_ID_SLAVE_BRIGHTNESS, MSG_ID_BRIGHTNESS_MAX});
    send_receive_check ({MSG_ID_SLAVE_INIT});
  }

  uart_t::~uart_t ()
  {
    device_close ();
  }

  bool uart_t::render (const core::matrix_t &info)
  {
    // fixme
    return true;
  }

  bool uart_t::brightness (int level)
  {
    // fixme
    return true;
  }

  void uart_t::device_open ()
  {
    m_descriptor = open (m_linux_device.c_str (), O_RDWR | O_NOCTTY | O_SYNC);
    if (m_descriptor < 0) {
      throw std::runtime_error ("Failed to open linux device");
    }
    if (!configure_attributes ()) {
      throw std::runtime_error ("Failed to configure attributes");
    }
    if (!configure_blocking ()) {
      throw std::runtime_error ("Failed to configure blocking");
    }
  }

  void uart_t::device_close ()
  {
    close (m_descriptor);
    m_descriptor = -1;
  }
  
  void uart_t::send_receive_check (const msg_t &msg)
  {
    // fixme
  }

  bool uart_t::configure_attributes ()
  {
    // fixme
    return true;
  }

  bool uart_t::configure_blocking ()
  {
    // fixme
    return true;
  }

} // namespace render
