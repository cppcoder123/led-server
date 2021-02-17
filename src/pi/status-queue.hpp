/*
 *
 */
#ifndef STATUS_QUEUE_HPP
#define STATUS_QUEUE_HPP

#include <condition_variable>
#include <functional>           // std::reference_wrapper
#include <memory>
#include <mutex>
#include <string>

#include "util/condition-queue.hpp"

#include "status.hpp"

namespace led_d
{
  using status_ptr_t = std::shared_ptr<status_t>;

  using status_queue_t =
    util::condition_queue_t<status_ptr_t,
                            std::reference_wrapper<std::mutex>,
                            std::reference_wrapper<std::condition_variable>>;
}

#endif
