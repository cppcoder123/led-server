/*
 *
 */
#ifndef MCU_DECODE_HPP
#define MCU_DECODE_HPP

#include <cstdint>

#include "const/constant.h"

#include "mcu-msg.hpp"

//
//
//

namespace led_d
{
  namespace mcu
  {
    class decode
    {
    public:

      static bool unwrap (mcu_msg_t &msg);

      template <typename ...payload_t>
      static bool split (const mcu_msg_t &msg, uint8_t &serial,
                         uint8_t &msg_id, payload_t& ...payload);

      template <typename ...payload_t>
      static bool split_payload (const mcu_msg_t &msg, payload_t& ...payload);

      // return true if decode was OK
      static bool get_serial (const mcu_msg_t &msg, uint8_t &serial);
      static bool get_msg_id (const mcu_msg_t &msg, uint8_t &msg_id);

    private:

      using iterator_t = mcu_msg_t::const_iterator;
      static bool do_split (const mcu_msg_t &msg, iterator_t &iter,
                            uint8_t &serial, uint8_t &msg_id);

      template <typename ...payload_t>
      static bool do_split (iterator_t &iter, iterator_t &end_iter,
                            uint8_t& load_1, payload_t& ...payload);

      static bool do_split (iterator_t&, iterator_t&) {return true;};

    };

    //
    // --------------------------------------------------
    //
    inline bool decode::unwrap (mcu_msg_t &msg)
    {
      while ((msg.empty () == false)
             && (msg.front () != EYE_CATCH))
        msg.pop_front ();

      if (msg.empty () == true)
        return false;

      // at least 4 elements: eye-catch, size, serial, msg_id
      if (msg.size () < 4)
        return false;

      mcu_msg_t::const_iterator iter = msg.cbegin ();
      ++iter;
      uint8_t size = *iter;

      if (size > msg.size () - 2)
        return false;

      msg.pop_front ();
      msg.pop_front ();

      return true;
    }

    template <typename ...payload_t>
    bool decode::split (const mcu_msg_t &msg, uint8_t &serial, uint8_t &msg_id,
                        payload_t& ...payload)
    {
      iterator_t iter;
      if (do_split (msg, iter, serial, msg_id) == false)
        return false;

      auto end_iter = msg.cend ();
      return do_split (iter, end_iter, payload...);
    }

    template <typename ...payload_t>
    bool decode::split_payload (const mcu_msg_t &msg, payload_t& ...payload)
    {
      uint8_t serial = 0;
      uint8_t msg_id = 0;
      return split (msg, serial, msg_id, payload...);
    }

    inline bool decode::get_serial (const mcu_msg_t &msg, uint8_t &serial)
    {
      uint8_t msg_id = 0;

      if (split (msg, serial, msg_id) == false)
        return false;

      return true;
    }

    inline bool decode::get_msg_id (const mcu_msg_t &msg, uint8_t &msg_id)
    {
      uint8_t serial = 0;

      if (split (msg, serial, msg_id) == false)
        return false;

      return true;
    }

    inline bool decode::do_split (const mcu_msg_t &msg, iterator_t &iter,
                                  uint8_t &serial, uint8_t &msg_id)
    {
      iter = msg.cbegin ();
      if (iter == msg.cend ())
        return false;
      serial = *iter;

      ++iter;
      if (iter == msg.cend ())
        return false;
      msg_id = *iter;

      return true;
    }

    template <typename ...payload_t>
    bool decode::do_split (iterator_t &iter, iterator_t &end_iter,
                           uint8_t& load_1, payload_t& ...payload)
    {
      ++iter;
      if (iter == end_iter)
        return false;

      load_1 = *iter;

      return do_split (iter, end_iter, payload...);
    }

  } // namespace mcu
} // namespace led_d

#endif
