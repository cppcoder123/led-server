//
//
//
#ifndef RENDER_UART_HPP
#define RENDER_UART_HPP

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

    bool start () override;
    void stop () override;

    bool render (const core::matrix_t &info) override;
    bool brightness (int level) override;
  };
  
} // namespace render

#endif
