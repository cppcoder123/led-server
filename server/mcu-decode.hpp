/*
 *
 */
#ifndef MCU_DECODE_HPP
#define MCU_DECODE_HPP

#include "mcu/constant.h"

#include "type-def.hpp"

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

      static bool unwrap (msg_t &msg);

      template <typename ...payload_t>
      static bool split (const msg_t &msg, char_t &serial,
                         char_t &msg_id, payload_t& ...payload);

      static char_t get_serial (const msg_t &msg);
      static char_t get_msg_id (const msg_t &msg);

    private:

      using iterator_t = msg_t::const_iterator;
      static bool do_split (const msg_t &msg, iterator_t &iter, char_t &serial, char_t &msg_id);

      template <typename ...payload_t>
      static bool do_split (const msg_t &msg, iterator_t &iter,
                            char_t& load_1, payload_t& ...payload);

      static bool do_split (const msg_t&, iterator_t&) {return true;};

    };

    //
    // --------------------------------------------------
    //
    inline bool decode::unwrap (msg_t &msg)
    {
      while ((msg.empty () == false)
             && (msg.front () != ID_CATCH_EYE))
        msg.pop_front ();

      if (msg.empty () == true)
        return false;

      // at least 4 elements: eye-catch, size, serial, msg_id
      if (msg.size () < 4)
        return false;

      msg_t::const_iterator iter = msg.cbegin ();
      ++iter;
      char_t size = *iter;

      if (size > msg.size () - 2)
        return false;

      msg.pop_front ();
      msg.pop_front ();

      return true;
    }

    template <typename ...payload_t>
    bool decode::split (const msg_t &msg, char_t &serial, char_t &msg_id,
                        payload_t& ...payload)
    {
      iterator_t iter;
      if (do_split (msg, iter, serial, msg_id) == false)
        return false;

      return do_split (msg, iter, payload...);
    }

    inline char_t decode::get_serial (const msg_t &msg)
    {
      char_t serial = 0;
      char_t msg_id = 0;

      if (split (msg, serial, msg_id) == false)
        return SERIAL_ID_TO_IGNORE;

      return serial;
    }

    inline char_t decode::get_msg_id (const msg_t &msg)
    {
      char_t serial = 0;
      char_t msg_id = 0;

      if (split (msg, serial, msg_id) == false)
        return false;

      return msg_id;
    }

    inline bool decode::do_split (const msg_t &msg, iterator_t &iter,
                                  char_t &serial, char_t &msg_id)
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
    bool decode::do_split (const msg_t &msg, iterator_t &iter,
                           char_t& load_1, payload_t& ...payload)
    {
      ++iter;
      if (iter == msg.cend ())
        return false;

      load_1 = *iter;

      return do_split (msg, iter, payload...);
    }

  } // namespace mcu
} // namespace led_d

#endif
