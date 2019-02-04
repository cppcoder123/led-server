/*
 *
 */
#ifndef HANDLE_HPP
#define HANDLE_HPP

#include "content.hpp"
#include "message-queue.hpp"
#include "spi.hpp"

namespace led_d
{

  class handle_t
  {
  public:
    handle_t (message_queue_t &message_queue, msg_queue_t &msg_queue,
              content_t &content, spi_t &spi);
    ~handle_t ();

    void start ();
    void stop ();
  };
  
} // namespace led_d

#endif
