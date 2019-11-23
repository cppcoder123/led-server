/*
 *
 */
#ifndef BASH_QUEUE_HPP
#define BASH_QUEUE_HPP

#include <condition_variable>
#include <functional>           // std::reference_wrapper
#include <memory>
#include <mutex>

#include "unix/condition-queue.hpp"

#include "bash-msg.hpp"

namespace led_d
{
  // bash related
  using bash_msg_ptr_t = std::unique_ptr<bash_msg_t>;

  using bash_queue_t
  = unix::condition_queue_t<bash_msg_ptr_t,
                            std::reference_wrapper<std::mutex>,
                            std::reference_wrapper<std::condition_variable>>;
} // namespace led_d

#endif
