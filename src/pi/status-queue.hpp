/*
 *
 */
#ifndef STATUS_QUEUE_HPP
#define STATUS_QUEUE_HPP

#include <condition_variable>
#include <functional>           // std::reference_wrapper
// #include <memory>
#include <mutex>
#include <string>

#include "unix/condition-queue.hpp"

namespace led_d
{
  struct status_t
  {
    unsigned id;
    int status;
    std::string result;
  };

  using status_queue_t =
    unix::condition_queue_t<status_t,
                            std::reference_wrapper<std::mutex>,
                            std::reference_wrapper<std::condition_variable>>;
}

#endif
