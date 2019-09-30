/*
 *
 */
#ifndef NETWORK_QUEUE_HPP
#define NETWORK_QUEUE_HPP

#include <condition_variable>
#include <functional>           // std::reference_wrapper
#include <memory>
#include <mutex>

#include "unix/condition-queue.hpp"

#include "network-msg.hpp"

namespace led_d
{
  // network related
  using network_msg_ptr_t = std::unique_ptr<network_msg_t>;

  using network_queue_t
  = unix::condition_queue_t<network_msg_ptr_t,
                            std::reference_wrapper<std::mutex>,
                            std::reference_wrapper<std::condition_variable>>;
} // namespace led_d

#endif
