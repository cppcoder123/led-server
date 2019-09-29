/*
 *
 */
#ifndef UNIX_QUEUE_HPP
#define UNIX_QUEUE_HPP

#include <memory>

#include "mutex-queue.hpp"
#include "unix-msg.hpp"

namespace led_d
{
  // unix related
  using unix_msg_ptr_t = std::unique_ptr<unix_msg_t>;
  using unix_queue_t = mutex::queue_t<unix_msg_ptr_t>;

} // namespace led_d

#endif
