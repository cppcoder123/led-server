/*
 *
 */
#ifndef COMMAND_ID_HPP
#define COMMAND_ID_HPP

namespace led_d
{
  namespace command_id
  {
    enum value_t {
      MPC_PLAY_LIST,              // mpc playlist
      MPC_PLAY_TRACK,             // mpc play 77
      STREAM_CLOCK,
      STREAM_SYSTEM,
      STREAM_TRACK_NAME,
    };

    inline bool stream (value_t id)
    {
      return ((id == STREAM_CLOCK)
              || (id == STREAM_SYSTEM)
              || (id == STREAM_TRACK_NAME)
              || (id == MPC_PLAY_LIST));
    }

    inline bool semi_stream (value_t id)
    {
      return (id == MPC_PLAY_LIST);
    }
  } // command_id

} // led_d


#endif
