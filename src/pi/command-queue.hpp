/*
 * Queue of commands for invoke bash
 */

#ifndef COMMAND_QUEUE_HPP
#define COMMAND_QUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <string>

#include "unix/condition-queue.hpp"

#include "command.hpp"

namespace led_d
{
  using command_queue_t =
    unix::condition_queue_t<command_t, std::mutex, std::condition_variable>;
  
} // led_d

#endif
