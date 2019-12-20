/*
 *
 */
#ifndef BASH_QUEUE_HPP
#define BASH_QUEUE_HPP

#include <condition_variable>
#include <functional>           // std::reference_wrapper
// #include <memory>
#include <mutex>
#include <string>

#include "unix/condition-queue.hpp"

namespace led_d
{
  using bash_queue_t
  = unix::condition_queue_t<std::string,
                            std::reference_wrapper<std::mutex>,
                            std::reference_wrapper<std::condition_variable>>;
} // namespace led_d

#endif
