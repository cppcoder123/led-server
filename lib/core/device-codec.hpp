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

      static constexpr char_t max_char = std::numeric_limits<char_t>::max ();

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
        // check the size: 2 chars for wrap, 1 - msg_id, 2 serial_id, so
        if (src.size () < 5)
          return false;

        char_t start = 0;
        
        return decode_data (src, std::ref (start), std::ref (msg_id), std::ref (serial_id))
          &&  (start == ID_MSG_START);
      }

      template <typename ...arg_t>
      static bool decode_tail (msg_t &src, arg_t ...arg)
      {
        bool decode_status = decode_data (src, arg...);
        if (decode_status == false)
          return false;

        // src should have 1 element
        if (src.size () != 1)
          return false;

        // and it should be equal to ID_MSG_FINISH
        char_t finish = src.front ();
        src.clear ();

        return finish == ID_MSG_FINISH;
      }

    private:

      static void encode_data (msg_t &msg)
      {
        msg.push_front (ID_MSG_START);
        msg.push_back (ID_MSG_FINISH);
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
        msg.push_back (info % max_char);
        msg.push_back (info / max_char);
      }
      static void encode_type (msg_t &msg, const msg_t &info)
      {
        msg.insert (msg.end (), info.begin (), info.end ());
      }

      static bool decode_data (const msg_t &src)
      {
        if (src.size () != 1)
          return false;

        char_t finish = *(src.begin ());

        return finish == ID_MSG_FINISH;
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
