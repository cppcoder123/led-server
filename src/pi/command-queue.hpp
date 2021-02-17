/*
 * Queue of commands to invoke bash
 */

#ifndef COMMAND_QUEUE_HPP
#define COMMAND_QUEUE_HPP

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>

#include "util/condition-queue.hpp"

#include "command.hpp"

namespace led_d
{
  using command_ptr_t = std::shared_ptr<command_t>;
  using command_queue_t =
    util::condition_queue_t<command_ptr_t, std::mutex, std::condition_variable>;
  
} // led_d

#endif
