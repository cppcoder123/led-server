/*
 *
 */
#include <sstream>

#include "unix/constant.h"
#include "unix/log.hpp"

#include "command-issue.hpp"
#include "command-queue.hpp"
#include "menu.hpp"

namespace led_d
{
  constexpr auto TRACK_ROTOR = ROTOR_1;     // select track
  constexpr auto VOLUME_ROTOR = ROTOR_2;    // tune volume
  constexpr auto MENU_ROTOR = VOLUME_ROTOR; // select menu
  // constexpr auto VALUE_ROTOR = TRACK_ROTOR; // select value

  constexpr auto CURRENT_TRACK = "mpc -f %position% current";
  constexpr auto START_TRACK = "mpc play ";

  constexpr auto LONG_DELAY = 15;
  constexpr auto SHORT_DELAY = 10;

  constexpr auto DISPLAY_CAPACITY = 8;

  menu_t::menu_t (asio::io_context &io_context,
                  status_queue_t &status_queue)
    : m_delta (0),
      m_playlist_update (false),
      // m_io_context (io_context),
      m_menu_timer (io_context),
      m_track_timer (io_context),
      m_status_queue (status_queue)
  {
  }

  void menu_t::command_queue (command_queue_t &command_queue)
  {
    m_command_queue = &command_queue;
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
        select_param (id_t::VOLUME);
      else 
        select_param (id_t::TRACK);
    }
    //
    switch (action) {
    case ROTOR_CLOCKWISE:
      ++m_delta;
      break;
    case ROTOR_COUNTER_CLOCKWISE:
      --m_delta;
      break;
    case ROTOR_PUSH:
      // volume & track directly accessible,
      // user wants something different
      // select (id_t::BRIGHTNESS);
      if (rotor_id == MENU_ROTOR)
        select ();
      else
        value ();
      break;
    default:
      log_t::buffer_t buf;
      buf << "menu: Unknown rotor action has arrived in idle mode \""
        << static_cast<int>(action) << "\"";
      log_t::error (buf);
      break;
    }

    display ();

    // switch (action) {
    // case ROTOR_CLOCKWISE:
    // case ROTOR_COUNTER_CLOCKWISE:
    //   {
    //     bool inc = (action == ROTOR_CLOCKWISE) ? true : false;
    //     if (rotor_id == MENU_ROTOR)
    //       select (inc);
    //     else
    //       value (inc);
    //   }
    //   break;
    // case ROTOR_PUSH:
    //   if (rotor_id == MENU_ROTOR)
    //     select ();
    //   else
    //     value ();
    //   break;
    // default:
    //   log_t::buffer_t buf;
    //   buf << "menu: Unknown rotor action has arrived in non idle mode \""
    //     << static_cast<int>(action) << "\"";
    //   log_t::error (buf);
    //   break;
    // }
  }

  void menu_t::volume_range (const std::string &low_str,
                             const std::string &high_str)
  {
    auto low = to_int (low_str);
    auto high = to_int (high_str);
    if ((!low) || (!high)) {
      log_t::buffer_t buf;
      buf << "menu: Failed to set volume range";
      log_t::error (buf);
      return;
    }

    m_volume_range = std::make_pair (*low, *high);
  }

  void menu_t::current_track (const std::string &src)
  {
    if ((!m_id) || (*m_id != id_t::TRACK))
      return;

    auto position = to_int (src);
    if (!position) {
      log_t::buffer_t buf;
      buf << "menu: Failed to convert \"" << src
          << "\" to track number";
      log_t::error (buf);
      return;
    }

    m_value = *position;

    display ();
  }

  void menu_t::current_volume (const std::string &src)
  {
    if ((!m_id) || (*m_id != id_t::VOLUME))
      return;

    auto level = to_int (src);
    if (!level) {
      log_t::buffer_t buf;
      buf << "menu: Failed to convert \"" << src
          << "\" to volume level";
      log_t::error (buf);
      return;
    }

    m_value = *level;

    display ();
  }

  void menu_t::select_param (id_t id)
  {
    m_id = id;
    m_delta = 0;
    m_value.reset ();
    set_range ();
    get_value ();
    // reflect ();                 // show menu info on display
  }

  void menu_t::select (bool inc)
  {
    select_param (inc_id (inc));
  }

  void menu_t::select ()
  {
    if (m_id)
      return;

    // we shouldn't get here,
    // but select menu that is not volume or track
    // fixme ???
    // select (id_t::BRIGHTNESS);
  }

  void menu_t::value (bool direction)
  {
    if ((!m_id) || (!m_value))
      return;

    if (!m_range) {
      log_t::buffer_t buf;
      buf << "menu: Empty range while handling menu value";
      log_t::error (buf);
      return;
    }

    auto old_value = *m_value;

    inc_value (direction);

    if ((old_value != *m_value)
        && (applyable () == true))
      set_value ();

    // reflect unconditionally to keep timer
    display ();
  }

  void menu_t::value ()
  {
    set_value ();
    //m_timer.cancel ()
    if (m_id)
      m_id.reset ();
    if (m_range)
      m_range.reset ();
    if (m_value)
      m_value.reset ();
  }

  void menu_t::set_range ()
  {
    if (!m_id)
      return;

    switch (*m_id) {
    // case id_t::BRIGHTNESS:
    //   m_range = std::make_pair (0, 15);
    //   break;
    case id_t::TRACK:
      if (m_playlist.empty () == false)
        m_range = std::make_pair (0, static_cast<int>(m_playlist.size () - 1));
      break;
    case id_t::VOLUME:
      if (m_volume_range)
        m_range = *m_volume_range;
      break;
    }
  }

  void menu_t::get_value ()
  {
    if (!m_id)
      return;

    switch (*m_id) {
    // case id_t::BRIGHTNESS:
    //   // fixme: not-implemented
    //   break;
    case id_t::TRACK:
      command_issue (command_id_t::MPC_CURRENT, CURRENT_TRACK,
                     command_t::three_seconds (), *m_command_queue);
      break;
    case id_t::VOLUME:
      command_issue (command_id_t::VOLUME_GET, m_volume_get,
                     command_t::three_seconds (), *m_command_queue);
      break;
    }
  }

  void menu_t::set_value ()
  {
    if (!m_id || !m_value)
      return;

    switch (*m_id) {
    // case id_t::BRIGHTNESS:
    //   // fixme: not-implemented
    //   break;
    case id_t::TRACK:
      {
        auto txt = std::string (START_TRACK) + std::to_string (*m_value);
        command_issue (command_id_t::MPC_PLAY, txt,
                       command_t::three_seconds (), *m_command_queue);
      }
      break;
    case id_t::VOLUME:
      {
        auto txt = m_volume_set + " " + std::to_string (*m_value);
        command_issue (command_id_t::VOLUME_SET, txt,
                       command_t::three_seconds (), *m_command_queue);
      }
      break;
    }
  }

  void menu_t::inc_value (bool dir)
  {
    if (!m_id || !m_value || !m_range)
      return;

    if (dir == true) {
      if (*m_value < m_range->second) {
        ++(*m_value);
        return;
      }
    } else {
      if (*m_value > m_range->first) {
        --(*m_value);
        return;
      }
    }

    if (wrapable () == true)
      m_value = (dir == true) ? m_range->first : m_range->second;
  }

  bool menu_t::wrapable () const
  {
    return (!m_id || (*m_id == id_t::VOLUME)) ? false : true;
  }

  bool menu_t::applyable () const
  {
    return (m_id && (*m_id == id_t::VOLUME)) ? true : false;
  }

  menu_t::id_t menu_t::inc_id (bool dir) const
  {
    id_t new_id = id_t::TRACK;

    if (!m_id)
      return new_id;

    switch (*m_id) {
    // case id_t::BRIGHTNESS:
    //   new_id = (dir == true) ? id_t::TRACK : id_t::VOLUME;
    //   break;
    case id_t::TRACK:
      new_id = id_t::VOLUME;
      break;
    case id_t::VOLUME:
      new_id = id_t::TRACK;
      break;
    }

    return new_id;
  }

  char menu_t::id_to_letter () const
  {
    if (!m_id)
      return ' ';

    switch (*m_id) {
    case id_t::TRACK:
      return 'T';
    case id_t::VOLUME:
      return 'V';
    default:
      return '?';
    }
  }

  void menu_t::display ()
  {
    std::string text;
    text += id_to_letter ();
    text += ' ';

    bool pos = (m_delta >= 0) ? true : false;
    text += (pos == true) ? '+' : '-';
    text += std::to_string ((pos == true) ? m_delta : -m_delta);

    if (m_value) {
      text += ' ';
      text += std::to_string (*m_value);
    }

    auto status = std::make_shared<status_t>
      (command_id_t::MENU_SET, status_t::good (), text);
    m_status_queue.push (status);

    m_menu_timer.expires_after (std::chrono::seconds (LONG_DELAY));
    m_menu_timer.async_wait
      (std::bind (&menu_t::menu_timeout, this, std::placeholders::_1));

    if ((m_id) && (*m_id == id_t::TRACK)) {
      m_track_timer.expires_after (std::chrono::seconds (SHORT_DELAY));
      m_track_timer.async_wait
        (std::bind (&menu_t::track_timeout, this, std::placeholders::_1));
    }
  }

  void menu_t::menu_timeout (const asio::error_code &error)
  {
    if (error)
      return;

    auto status = std::make_shared<status_t>
      (command_id_t::MENU_SET, status_t::good (), "");
    m_status_queue.push (status);
  }

  void menu_t::track_timeout (const asio::error_code &error)
  {
    if (error)
      return;

    if ((!m_id) || (*m_id != id_t::TRACK) || (!m_value) || (!m_range))
      return;

    // ?
    if ((*m_value < 0)
        || (static_cast<std::size_t>(*m_value) >= m_playlist.size ()))
      return;

    auto track_name = std::string (" ") + m_playlist[*m_value];
    // std::string track_name;
    // if (raw.size () == DISPLAY_CAPACITY)
    //   track_name = raw;
    // else if (raw.size () < DISPLAY_CAPACITY)
    //   track_name = raw + std::string (DISPLAY_CAPACITY - raw.size (), ' ');
    // else
    //   raw.substr (0, DISPLAY_CAPACITY);

    auto status = std::make_shared<status_t>
      (command_id_t::MENU_ADD, status_t::good (), track_name);
    m_status_queue.push (status);
  }

  std::optional<menu_t::value_t> menu_t::to_int (const std::string &src)
  {
    std::istringstream stream (src);
    menu_t::value_t val = 0;
    stream >> val;

    if (stream.fail () == true) {
      log_t::buffer_t buf;
      buf << "menu: Failed to convert \"" << src << "\" to integer";
      log_t::error (buf);
      return {};
    }

    return val;
  }
} // led_d
