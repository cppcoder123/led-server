/*
 *
 */
#ifndef TYPE_DEF_HPP
#define TYPE_DEF_HPP

#include "mcu/codec.hpp"

#include "mutex-queue.hpp"

namespace led_d
{
  using char_t = mcu::codec_t::char_t;
  using msg_t = mcu::codec_t::msg_t;
  using msg_queue_t = mutex::queue_t<msg_t>;
} // namespace led_d

#endif
