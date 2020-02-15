/*
 *
 */
#ifndef COMMAND_ID_HPP
#define COMMAND_ID_HPP

#include <string>

namespace led_d
{
  enum class command_id_t {
    MPC_PLAY,           // mpc play
    MPC_PLAYLIST,       // mpc playlist - show list of tracks/stations
    MPC_CURRENT,        // current track
    POWEROFF,           // self shutdown
    STREAM_CLOCK,
    STREAM_SYSTEM,
    STREAM_TRACK_NAME,
    VOLUME_GET,
    VOLUME_SET,
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

  inline std::string command_id_name (command_id_t id)
  {
    switch (id) {
    case command_id_t::MPC_PLAY:
      return "mpc-play";
      break;
    case command_id_t::MPC_PLAYLIST:
      return "mpc-playlist";
      break;
    case command_id_t::MPC_CURRENT:
      return "mpc-current";
      break;
    case command_id_t::POWEROFF:
      return "poweroff";
      break;
    case command_id_t::STREAM_CLOCK:
      return "stream-clock";
      break;
    case command_id_t::STREAM_SYSTEM:
      return "stream-system";
      break;
    case command_id_t::STREAM_TRACK_NAME:
      return "stream-track-name";
      break;
    case command_id_t::VOLUME_GET:
      return "volume-get";
      break;
    case command_id_t::VOLUME_SET:
      return "volume-set";
      break;
    }

    return "";
  }
} // led_d


#endif
