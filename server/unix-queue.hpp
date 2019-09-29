/*
 *
 */
#ifndef UNIX_QUEUE_HPP
#define UNIX_QUEUE_HPP

#include <condition_variable>
#include <functional>           // std::reference_wrapper
#include <memory>
#include <mutex>

#include "unix/condition-queue.hpp"

#include "mutex-queue.hpp"
#include "unix-msg.hpp"

namespace led_d
{
  // unix related
  using unix_msg_ptr_t = std::unique_ptr<unix_msg_t>;
  using unix_queue_t = mutex::queue_t<unix_msg_ptr_t>;

  using unix_queue_a_t
  = unix::condition_queue_t<unix_msg_ptr_t,
                            std::reference_wrapper<std::mutex>,
                            std::reference_wrapper<std::condition_variable>>;

} // namespace led_d

#endif
