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
      static void encode (msg_t &dst, char_t msg_id, short_t serial_id, arg_t ...arg)
      {
        encode_data (dst, msg_id, serial_id, arg...);
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
    };
    
  } // namespace device
} // namespace core


#endif
