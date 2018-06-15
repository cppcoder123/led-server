//
// Establish connection with arduino
//

#include <chrono>
#include <thread>

#include "initial.hpp"
#include "log-wrapper.hpp"

namespace led_d
{

  void initial_t::start (serial_t &serial)
  {
    {
      log_t::buffer_t buf;
      buf << "Trying to establish device connection";
      log_t::info (buf);
    }
    
    while (serial.handshake () == true)
      std::this_thread::sleep_for (std::chrono::milliseconds (300));

    {
      log_t::buffer_t buf;
      buf << "Device connection is established, detaching initial thread";
      log_t::info (buf);
    }
  }

} // namespace led_d
