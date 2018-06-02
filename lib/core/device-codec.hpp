//
// Encode/decode device messages
//
#ifndef CORE_DEVICE_CODEC_HPP
#define CORE_DEVICE_CODEC_HPP

#include <functional>
#include <limits>
#include <list>

#include "device-id.h"

namespace core
{
  namespace device
  {

    class codec_t
    {
    public:

      using char_t = unsigned char;
      using msg_t = std::list<char_t>;
      using short_t = unsigned short;

      static constexpr short_t max_char = std::numeric_limits<char_t>::max () + 1;

      template <typename arg_t>
      static char_t to_char (arg_t arg)
      {
        return static_cast<char_t>(arg);
      }

      template <typename arg_t>
      static short_t to_short (arg_t arg)
      {
        return static_cast<short_t>(arg);
      }


      template <typename ...arg_t>
      static msg_t encode (char_t msg_id, short_t serial_id, arg_t ...arg)
      {
        msg_t dst;
        encode_data (dst, msg_id, serial_id, arg...);
        return dst;
      }


      static bool decode_head (msg_t &src, char_t &msg_id, short_t &serial_id)
      {
        // msg-id + serial-id (2 bytes), so
        if (src.size () < 3)
          return false;

        return decode_data (src, std::ref (msg_id), std::ref (serial_id));
      }

      template <typename ...arg_t>
      static bool decode_body (msg_t &src, arg_t ...arg)
      {
        return decode_data (src, arg...);
      }

    private:

      static void encode_data (msg_t &msg)
      {
        // no-op
      }

      template <typename first_t, typename ...arg_t>
      static void encode_data (msg_t &msg,
                               first_t first, const arg_t ...arg)
      {
        encode_type (msg, first);
        encode_data (msg, arg...);
      }
      
      static void encode_type (msg_t &msg, char_t info)
      {
        msg.push_back (info);
      }
      static void encode_type (msg_t &msg, short_t info)
      {
        msg.push_back (static_cast<char_t>(info % max_char));
        msg.push_back (static_cast<char_t>(info / max_char));
      }
      static void encode_type (msg_t &msg, const msg_t &info)
      {
        msg.insert (msg.end (), info.begin (), info.end ());
      }

      static bool decode_data (const msg_t &src)
      {
        // no-op
        return true;
      }
      
      template <typename first_t, typename ...arg_t>
      static bool decode_data (msg_t &src, first_t first, arg_t ...arg)
      {
        if (decode_type (src, first) == false)
          return false;

        return decode_data (src, arg...);
      }

      static bool decode_type (msg_t &src, char_t &info)
      {
        if (src.size () < 1)
          return false;

        info = src.front ();
        src.pop_front ();

        return true;
      }

      static bool decode_type (msg_t &src, short_t &info)
      {
        if (src.size () < 2)
          return false;

        char_t lsb = src.front ();
        src.pop_front ();
        char_t msb = src.front ();
        src.pop_front ();

        info  = msb * max_char + lsb;

        return true;
      }
    };
    
  } // namespace device
} // namespace core


#endif
