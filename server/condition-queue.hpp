//
//
//
#ifndef CONDITION_QUEUE_HPP
#define CONDITION_QUEUE_HPP

#include <condition_variable>
#include <optional>

#include "mutex-queue.hpp"

namespace condition
{

  template <typename record_t>
  class queue_t : public mutex::queue_t<record_t>
  {
  public:
    queue_t ();
    ~queue_t () {};

    using base_queue_t = mutex::queue_t<record_t>;

    void push (record_t record) {base_queue_t::push (std::move (record));};
    std::optional<record_t> pop () {return base_queue_t::pop ();};

  private:
    std::condition_variable m_condition;
  };

  template <typename record_t>
  queue_t<record_t>::queue_t ()
    : base_queue_t()
  {
    base_queue_t::set_notify_wait ([this] () {m_condition.notify_one ();},
      [this] (typename base_queue_t::unique_lock_t &lock) {m_condition.wait (lock);});
  }

} // namespace condition

#endif
