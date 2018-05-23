//
//
//
#ifndef LED_D_MESSAGE_QUEUE_HPP
#define LED_D_MESSAGE_QUEUE_HPP

#include "message-ptr.hpp"
#include "move-queue.hpp"

namespace led_d
{
  using queue_t = move::queue_t<message_ptr_t>;
} // namespace led_d

#endif
