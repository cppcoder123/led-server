//
// Encode/decode device messages
//
#ifndef MCU_CODEC_HPP
#define MCU_CODEC_HPP

#include <limits>
#include <list>

namespace mcu
{
  class codec_t
  {
  public:

    using char_t = unsigned char;
    using msg_t = std::list<char_t>;

    template <typename ...arg_t>
    static msg_t encode (char_t first, arg_t ...arg)
    {
      msg_t dst;
      encode_data (dst, first, arg...);
      return dst;
    }

    template <typename ...arg_t>
    static bool decode_modify (msg_t &src, arg_t ...arg)
    {
      return decode_data (src, arg...);
    }

    template <typename ...arg_t>
    static bool decode (const msg_t &src, arg_t ...arg)
    {
      msg_t src_copy = src;

      return decode_modify (src_copy, arg...);
    }      

    // static bool decode_head (msg_t &src, char_t &msg_id, char_t &serial_id)
    // {
    //   // msg-id + serial-id (1 bytes), so
    //   if (src.size () < 1)
    //     return false;

    //   return decode_data (src, std::ref (msg_id), std::ref (serial_id));
    // }

    // template <typename ...arg_t>
    // static bool decode_body (msg_t &src, arg_t ...arg)
    // {
    //   return decode_data (src, arg...);
    // }

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
    // static void encode_type (msg_t &msg, short_t info)
    // {
    //   msg.push_back (static_cast<char_t>(info % max_char));
    //   msg.push_back (static_cast<char_t>(info / max_char));
    // }
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

    // static bool decode_type (msg_t &src, short_t &info)
    // {
    //   if (src.size () < 2)
    //     return false;

    //   char_t lsb = src.front ();
    //   src.pop_front ();
    //   char_t msb = src.front ();
    //   src.pop_front ();

    //   info  = msb * max_char + lsb;

    //   return true;
    // }
  };
    
} // namespace mcu


#endif
