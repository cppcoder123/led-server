//
//
//
#ifndef RENDER_UART_HPP
#define RENDER_UART_HPP

#include <list>
#include <cstdint>
#include <string>

#include "matrix.hpp"

#include "device.hpp"

namespace render
{
  class uart_t : public device_t
  {
  public:
    uart_t () = delete;
    uart_t (const std::string &linux_device);
    ~uart_t ();

    bool render (const core::matrix_t &info) override;
    bool brightness (int level) override;

  private:
    void device_open ();
    void device_close ();

    using msg_t = std::list<std::uint8_t>;
    void send_receive_check (const msg_t &msg);

    bool configure_attributes ();
    bool configure_blocking ();

    const std::string m_linux_device;
    int m_descriptor;
  };
  
} // namespace render

#endif
