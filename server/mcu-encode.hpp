/*
 *
 */
#ifndef MCU_ENCODE_HPP
#define MCU_ENCODE_HPP

#include "type-def.hpp"

namespace led_d
{
  namespace mcu
  {

    class encode
    {
    public:

      static void wrap (msg_t &msg);

      template <typename ...payload_t>
      static msg_t join (char_t serial, char_t msg_id, payload_t ...payload);

    private:

      static void do_join (msg_t &msg) {};

      template <typename ...payload_t>
      static void do_join (msg_t &msg, char_t load_1, payload_t ...payload);
    
    };

    //
    // --------------------------------------------------
    //
    inline void encode::wrap (msg_t &msg)
    {
      msg.push_front (msg.size ());
      msg.push_front (ID_CATCH_EYE);
    }

    template <typename ...payload_t>
    msg_t encode::join (char_t serial, char_t msg_id, payload_t ...payload)
    {
      msg_t msg;
      msg.push_back (serial);
      msg.push_back (msg_id);

      do_join (msg, payload...);

      return msg;
    }

    template <typename ...payload_t>
    void encode::do_join (msg_t &msg, char_t load_1, payload_t ...payload)
    {
      msg.push_back (load_1);

      do_join (msg, payload...);
    }

  } // namespace mcu
} // namespace led_d

#endif
