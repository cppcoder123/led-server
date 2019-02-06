/*
 *
 */
#ifndef TYPE_DEF_HPP
#define TYPE_DEF_HPP

#include <cstdint>
#include <list>
#include <memory>

#include "mutex-queue.hpp"
#include "unix-msg.hpp"

namespace led_d
{
  // mcu related
  using char_t = uint8_t;
  using mcu_msg_t = std::list<char_t>;
  using mcu_queue_t = mutex::queue_t<mcu_msg_t>;

  // unix related
  using unix_msg_ptr_t = std::unique_ptr<unix_msg_t>;
  using unix_queue_t = mutex::queue_t<unix_msg_ptr_t>;

} // namespace led_d

#endif
