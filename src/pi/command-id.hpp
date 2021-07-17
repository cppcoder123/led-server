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
    MPC_PLAYLIST_GET,   // get playlist info
    MPC_PLAYLIST_NAME,  // get playlist name
    MPC_PLAYLIST_SET,   // load playlist
    MPC_STOP,           // stop
    MPC_TRACK_GET,      // current track
    MPC_TRACK_NAME,     // get track name
    MPC_TRACK_SET,
    MPC_VOLUME_GET,
    MPC_VOLUME_SET,
    POWEROFF,           // shutdown immediately
    SHUTDOWN,           // shutdown with delay
    REBOOT,             // reboot
    STREAM_CLOCK,
    STREAM_SYSTEM,
    STREAM_TRACK_NAME,
  };

  inline bool command_id_stream (command_id_t id)
  {
    return ((id == command_id_t::STREAM_CLOCK)
            || (id == command_id_t::STREAM_SYSTEM)
            || (id == command_id_t::STREAM_TRACK_NAME));
  }

  inline std::string command_id_name (command_id_t id)
  {
    switch (id) {
    case command_id_t::MPC_PLAY:
      return "mpc-play";
      break;
    case command_id_t::MPC_PLAYLIST_GET:
      return "mpc-playlist-get";
      break;
    case command_id_t::MPC_PLAYLIST_NAME:
      return "mpc-playlist-name";
      break;
    case command_id_t::MPC_PLAYLIST_SET:
      return "mpc-playlist-set";
      break;
    case command_id_t::MPC_STOP:
      return "mpc-stop";
      break;
    case command_id_t::MPC_TRACK_GET:
      return "mpc-track-get";
      break;
    case command_id_t::MPC_TRACK_NAME:
      return "mpc-track-name";
      break;
    case command_id_t::MPC_TRACK_SET:
      return "mpc-track-set";
      break;
    case command_id_t::MPC_VOLUME_GET:
      return "mpc-volume-get";
      break;
    case command_id_t::MPC_VOLUME_SET:
      return "mpc-volume-set";
      break;
    case command_id_t::POWEROFF:
      return "poweroff";
      break;
    case command_id_t::SHUTDOWN:
      return "shutdown";
      break;
    case command_id_t::REBOOT:
      return "reboot";
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
    }

    return "";
  }
} // led_d


#endif
