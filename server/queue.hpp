//
//
//
#ifndef LED_D_QUEUE_HPP
#define LED_D_QUEUE_HPP

#include "message-ptr.hpp"
#include "meta-queue.hpp"

namespace led_d
{
  namespace queue_wrap
  {

    template <typename record_t>
    struct queue_t : public meta::queue_t<record_t>
    {
      void push (record_t &&record)
      {meta::queue_t<record_t>::move_push (record);}
      bool pop (record_t &record)
      {return meta::queue_t<record_t>::move_pop (record);}
    };

  } // namespace queue_wrap

  typedef queue_wrap::queue_t<message_ptr_t> queue_t;

  
  //
  // We have the only queue in the project, so
  //
  inline queue_t& queue_buffer ()
  {
    static queue_t queue;

    return queue;
  }

  
} // namespace led_d

#endif
