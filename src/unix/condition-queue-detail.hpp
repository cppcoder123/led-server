/*
 *
 */

#ifndef UNIX_CONDITION_QUEUE_DETAIL_HPP
#define UNIX_CONDITION_QUEUE_DETAIL_HPP

#include <condition_variable>
#include <mutex>

namespace unix
{
  namespace condition_queue_detail
  {

    template <bool really_wait>
    void wait (std::unique_lock<std::mutex> &lock, std::condition_variable &condition)
    {
      condition.wait (lock);
    }

    template <>
    inline void wait<false> (std::unique_lock<std::mutex>&, std::condition_variable&)
    {
      // Don't wait
    }

    struct dummy_lock_t
    {
      dummy_lock_t (std::mutex&) {}
      ~dummy_lock_t () = default;
    };

    template <bool really_lock>
    struct guard
    {
      using lock = std::lock_guard<std::mutex>;
    };

    template <>
    struct guard<false>
    {
      using lock = dummy_lock_t;
    };

    // struct dummy_unique_t
    // {
    //   dummy_unique_t (std::mutex&) = default;
    //   ~dummy_unique_t () = default;
    // };

    template <bool really_lock>
    struct unique
    {
      using lock = std::unique_lock<std::mutex>;
    };

    template <>
    struct unique<false>
    {
      using lock = dummy_lock_t;
    };

  } // condition_queue_detail
} // unix

#endif
