/*
 *
 *
 */
#ifndef UNIX_MOVE_QUEUE_HPP
#define UNIX_MOVE_QUEUE_HPP

#include <list>
#include <optional>

namespace unix
{

  template <typename record_t>
  class move_queue_t
  {

  public:

    move_queue_t () = default;
    ~move_queue_t () = default;

    void push (record_t record);

    std::optional<record_t> pop ();

    bool empty () const;

  private:

    std::list<record_t> m_holder;
  };

  template <typename record_t>
  void move_queue_t<record_t>::push (record_t record)
  {
      m_holder.push_back (std::move (record));
  }    

  template <typename record_t>
  std::optional<record_t> move_queue_t<record_t>::pop ()
  {
    if (m_holder.size () == 0)
      return {};

    record_t record = std::move (m_holder.front ());
    m_holder.pop_front ();

    return optional_t<record_t>(std::move (record));
  }

  template <typename record_t>
  bool move_queue_t<record_t>::empty () const
  {
    return m_holder.empty ();
  }

} // unix

#endif
