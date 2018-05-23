//
//
//
#ifndef LED_D_QUEUE_HPP
#define LED_D_QUEUE_HPP

#include "meta-queue.hpp"

namespace led_d
{
  namespace move
  {

    template <typename record_t>
    struct queue_t : public meta::queue_t<record_t>
    {
      void push (record_t &&record)
      {meta::queue_t<record_t>::move_push (record);}
      bool pop (record_t &record)
      {return meta::queue_t<record_t>::move_pop (record);}
    };

  } // namespace move

} // namespace led_d

#endif
