/*
 *
 */

#include "unix/constant.h"
#include "unix/log.hpp"

#include "menu.hpp"

// current track number
// mpc -f %position% current

namespace led_d
{
  constexpr auto TRACK_ROTOR = ROTOR_1;     // select track
  constexpr auto VOLUME_ROTOR = ROTOR_2;    // tune volume
  constexpr auto MENU_ROTOR = VOLUME_ROTOR; // select menu
  constexpr auto VALUE_ROTOR = TRACK_ROTOR; // select value

  menu_t::menu_t ()
    : m_playlist_update (false)
  {
  }

  void menu_t::track_add (const std::string &track)
  {
    if (m_playlist_update == false) {
      m_playlist_update = true;
      m_playlist.clear ();
    }
    if (track.empty () == false) {
      m_playlist.push_back (track);
    } else {
      m_playlist_update = false;
    }
  }

  void menu_t::track_clear ()
  {
    m_playlist.clear ();
  }

  void menu_t::rotor (uint8_t rotor_id, uint8_t action)
  {
    if (!m_id) {
      if (rotor_id == VOLUME_ROTOR)
        select (id_t::VOLUME);
      else 
        select (id_t::TRACK);
      //
      switch (action) {
      case ROTOR_CLOCKWISE:
        value (true);
        break;
      case ROTOR_COUNTER_CLOCKWISE:
        value (false);
        break;
      case ROTOR_PUSH:
        // volume & track directly accessible,
        // user wants something different
        select (id_t::BRIGHTNESS);
        break;
      default:
        log_t::buffer_t buf;
        buf << "menu: Unknown rotor action has arrived in idle mode \""
          << static_cast<int>(action) << "\"";
        log_t::error (buf);
        break;
      }
      return;
    }

    switch (action) {
    case ROTOR_CLOCKWISE:
    case ROTOR_COUNTER_CLOCKWISE:
      {
        bool inc = (action == ROTOR_CLOCKWISE) ? true : false;
        if (rotor_id == MENU_ROTOR)
          select (inc);
        else
          value (inc);
      }
      break;
    case ROTOR_PUSH:
      if (rotor_id == MENU_ROTOR)
        select ();
      else
        value ();
      break;
    default:
      log_t::buffer_t buf;
      buf << "menu: Unknown rotor action has arrived in non idle mode \""
        << static_cast<int>(action) << "\"";
      log_t::error (buf);
      break;
    }
  }
  
} // led_d
