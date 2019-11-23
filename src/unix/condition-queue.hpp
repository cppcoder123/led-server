/*
 *
 */
#ifndef UNIX_CONDITION_QUEUE_HPP
#define UNIX_CONDITION_QUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <optional>

#include "move-queue.hpp"

namespace unix
{

  // mutex_t is either std::mutex or std::ref(std::mutex)
  // similar for condition_t
  template <typename record_t, typename mutex_t, typename condition_t>
  class condition_queue_t
  {

  public:

    condition_queue_t () = default;
    condition_queue_t (mutex_t mutex, condition_t condition);
    ~condition_queue_t () = default;

    void push (record_t record);

    template <bool wait>
    std::optional<record_t> pop ();

    template <bool lock>
    bool empty ();

    template <bool lock>
    void notify_one ();

  private:

    mutex_t m_mutex;
    condition_t m_condition;

    move_queue_t<record_t> m_queue;
  };

  template <typename record_t, typename mutex_t, typename condition_t>
  condition_queue_t<record_t, mutex_t, condition_t>::
  condition_queue_t (mutex_t mutex, condition_t condition)
    : m_mutex (mutex),
      m_condition (condition)
  {
  }

  template <typename record_t, typename mutex_t, typename condition_t>
  void condition_queue_t<record_t, mutex_t, condition_t>::push (record_t record)
  {
    // take a reference
    std::mutex &mutex (m_mutex);
    std::lock_guard lock (mutex);

    bool empty = m_queue.empty ();

    m_queue.push (std::move (record));

    if (empty == true) {
      std::condition_variable &condition (m_condition);
      condition.notify_one ();
    }
  }

  template <typename record_t, typename mutex_t, typename condition_t>
  template <bool really_wait>
  std::optional<record_t>
  condition_queue_t<record_t, mutex_t, condition_t>::pop ()
  {
    std::mutex &mutex (m_mutex);
    std::unique_lock lock (mutex);
    std::condition_variable &condition (m_condition);

    if constexpr (really_wait)
      condition.wait (lock);
    
    if (m_queue.empty ())
      return {};

    return std::optional<record_t>(m_queue.pop ());
  }

  template <typename record_t, typename mutex_t, typename condition_t>
  template <bool really_lock>
  bool condition_queue_t<record_t, mutex_t, condition_t>::empty ()
  {
    if constexpr (really_lock)
      std::lock_guard lock (m_mutex);

    return m_queue.empty ();
  }

  template <typename record_t, typename mutex_t, typename condition_t>
  template <bool really_lock>
  void condition_queue_t<record_t, mutex_t, condition_t>::notify_one ()
  {
    if constexpr (really_lock)
      std::lock_guard lock (m_mutex);

    m_condition.notify_one ();
  }

} // unix

#endif
