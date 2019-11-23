/*
 *
 */
#ifndef MCU_ENCODE_HPP
#define MCU_ENCODE_HPP

#include <cstdint>

#include "unix/constant.h"

#include "mcu-msg.hpp"

namespace led_d
{
  namespace mcu
  {

    class encode
    {
    public:

      static void wrap (mcu_msg_t &msg);

      template <typename ...payload_t>
      static mcu_msg_t join (uint8_t serial,
                             uint8_t msg_id, payload_t ...payload);

    private:

      static void do_join (mcu_msg_t &msg) {};

      template <typename ...payload_t>
      static void do_join (mcu_msg_t &msg, uint8_t load_1, payload_t ...payload);

      template <typename ...payload_t>
      static void do_join (mcu_msg_t &msg, mcu_msg_t &arr, payload_t ...payload);

    };

    //
    // --------------------------------------------------
    //
    inline void encode::wrap (mcu_msg_t &msg)
    {
      msg.push_front (msg.size ());
      msg.push_front (EYE_CATCH);
    }

    template <typename ...payload_t>
    mcu_msg_t encode::join (uint8_t serial, uint8_t msg_id, payload_t ...payload)
    {
      mcu_msg_t msg;
      msg.push_back (serial);
      msg.push_back (msg_id);

      do_join (msg, payload...);

      return msg;
    }

    template <typename ...payload_t>
    void encode::do_join (mcu_msg_t &msg, uint8_t load_1, payload_t ...payload)
    {
      msg.push_back (load_1);

      do_join (msg, payload...);
    }

    template <typename ...payload_t>
    void encode::do_join (mcu_msg_t &msg, mcu_msg_t &arr, payload_t ...payload)
    {
      msg.splice (msg.end (), arr);

      do_join (msg, payload...);
    }

  } // namespace mcu
} // namespace led_d

#endif
