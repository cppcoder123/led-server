//
//
//
#ifndef LED_D_MESSAGE_QUEUE_HPP
#define LED_D_MESSAGE_QUEUE_HPP

#include "message-ptr.hpp"
#include "mutex-queue.hpp"

namespace led_d
{
  using message_queue_t = mutex::queue_t<message_ptr_t>;
} // namespace led_d

#endif
