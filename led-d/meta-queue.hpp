//
// either copy or move push/pop arg
//
// mutex, condition and list
//

#ifndef META_QUEUE_HPP
#define META_QUEUE_HPP

#include <condition_variable>
#include <list>
#include <mutex>


namespace meta
{

  template <typename record_t> 
  class queue_t
  {
    
  public:

    queue_t ();
    queue_t (const queue_t &) = delete;
    ~queue_t ();

    void clear ();

    void notify_one () const;

    bool empty () const;

  protected:

    // put a record into the queue
    void copy_push (const record_t &record);
    void move_push (record_t &record);

    // extract record from a queue
    bool copy_pop (record_t &record);
    bool move_pop (record_t &record);

  private:
    
    mutable std::mutex m_mutex;
    mutable std::condition_variable m_condition;

    typedef std::list<record_t> holder_t;
    holder_t m_holder;
  };
  
  template <typename record_t>
  queue_t<record_t>::queue_t ()
  {
  }

  template <typename record_t>
  queue_t<record_t>::~queue_t ()
  {
  }

  template <typename record_t> 
  void queue_t<record_t>::copy_push (const record_t &record)
  {
    std::size_t len (0);

    {
      std::lock_guard<std::mutex> lock (m_mutex);
      len = m_holder.size ();
      m_holder.push_back (record);
    }

    if (len == 0)
      m_condition.notify_one ();
  }

  template <typename record_t>
  bool queue_t<record_t>::copy_pop (record_t &record)
  {
    std::unique_lock<std::mutex> lock (m_mutex);

    if (m_holder.size () == 0)
      m_condition.wait (lock);

    if (m_holder.size () == 0)
      return false;

    record = m_holder.front ();
    m_holder.pop_front ();

    return true;
  }

  template <typename record_t> 
  void queue_t<record_t>::move_push (record_t &record)
  {
    std::size_t len (0);

    {
      std::lock_guard<std::mutex> lock (m_mutex);
      len = m_holder.size ();
      m_holder.push_back (std::move (record));
    }

    if (len == 0)
      m_condition.notify_one ();
  }

  template <typename record_t>
  bool queue_t<record_t>::move_pop (record_t &record)
  {
    std::unique_lock<std::mutex> lock (m_mutex);

    if (m_holder.size () == 0)
      m_condition.wait (lock);

    if (m_holder.size () == 0)
      return false;

    record = std::move (m_holder.front ());
    m_holder.pop_front ();

    return true;
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
    m_condition.notify_one ();
  }

  template <typename record_t>
  bool queue_t<record_t>::empty () const
  {
    std::lock_guard <std::mutex> lock (m_mutex);
    return m_holder.empty ();
  }
  
} // namespace meta

#endif
