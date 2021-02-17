/*
 *
 */
#ifndef MCU_QUEUE_HPP
#define MCU_QUEUE_HPP

#include "util/condition-queue.hpp"

#include "mcu-msg.hpp"

namespace led_d
{
  using mcu_queue_t = util::condition_queue_t
    <mcu_msg_t, std::reference_wrapper<std::mutex>,
     std::reference_wrapper<std::condition_variable>>;
} // led_d

#endif
