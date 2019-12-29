/*
 *
 */
#ifndef COMMAND_ID_HPP
#define COMMAND_ID_HPP

namespace led_d
{
    enum class command_id_t {
      MPC_PLAY,          // mpc play - play last track
      MPC_PLAYLIST,      // mpc playlist - show list of tracks/stations
      STREAM_CLOCK,
      STREAM_SYSTEM,
      STREAM_TRACK_NAME,
    };

    inline bool command_id_stream (command_id_t id)
    {
      return ((id == command_id_t::STREAM_CLOCK)
              || (id == command_id_t::STREAM_SYSTEM)
              || (id == command_id_t::STREAM_TRACK_NAME)
              || (id == command_id_t::MPC_PLAYLIST));
    }

    inline bool command_id_semi_stream (command_id_t id)
    {
      return (id == command_id_t::MPC_PLAYLIST);
    }

} // led_d


#endif
