//
// either copy or move push/pop arg
//
// mutex and list
//

#ifndef MUTEX_QUEUE_HPP
#define MUTEX_QUEUE_HPP

#include <functional>
#include <list>
#include <mutex>
#include <optional>


namespace mutex
{

  template <typename record_t> 
  class queue_t
  {
    
  public:

    using notify_t = std::function<void ()>;

    using mutex_t = std::mutex;
    using unique_lock_t = std::unique_lock<mutex_t>;
    using wait_t = std::function<void (unique_lock_t&)>;
    
    queue_t ();
    queue_t (const queue_t &) = delete;
    ~queue_t ();

    void set_notify_wait (notify_t notify, wait_t wait);
    
    void clear ();

    void notify_one () const;

    bool empty () const;

  protected:

    // put a record into the queue
    void push (record_t record);

    // extract record from a queue
    std::optional<record_t> pop ();

  private:
    
    mutable mutex_t m_mutex;

    using holder_t = std::list<record_t>;
    holder_t m_holder;

    notify_t m_notify;
    wait_t m_wait;
  };
  
  template <typename record_t>
  queue_t<record_t>::queue_t ()
    : m_notify ([](){}),
      m_wait ([](unique_lock_t&){})
  {
  }

  template <typename record_t>
  queue_t<record_t>::~queue_t ()
  {
  }

  template <typename record_t>
  void queue_t<record_t>::set_notify_wait (notify_t notify, wait_t wait)
  {
    m_notify = notify;
    m_wait = wait;
  }

  template <typename record_t> 
  void queue_t<record_t>::push (record_t record)
  {
    std::size_t len (0);

    {
      std::lock_guard<mutex_t> lock (m_mutex);
      len = m_holder.size ();
      m_holder.push_back (std::move (record));
    }

    if (len == 0)
      m_notify ();
  }

  template <typename record_t>
  std::optional<record_t> queue_t<record_t>::pop ()
  {
    std::unique_lock<std::mutex> lock (m_mutex);

    if (m_holder.size () == 0)
      m_wait (lock);

    if (m_holder.size () == 0)
      return {};

    record_t record = std::move (m_holder.front ());
    m_holder.pop_front ();

    return std::optional<record_t>(std::move (record));
  }

  template <typename record_t>
  void queue_t<record_t>::clear ()
  {
    std::lock_guard <std::mutex> lock (m_mutex);
    m_holder.clear ();
  }

  template <typename record_t>
  void queue_t<record_t>::notify_one () const
  {
    std::unique_lock<std::mutex> guard (m_mutex);
    m_notify ();
  }

  template <typename record_t>
  bool queue_t<record_t>::empty () const
  {
    std::lock_guard <std::mutex> lock (m_mutex);
    return m_holder.empty ();
  }
  
} // namespace mutex

#endif
