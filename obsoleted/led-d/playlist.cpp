/*
 *
 */

#include "playlist.hpp"

namespace led_d
{

  playlist_t::playlist_t (/*asio::io_context &io_context*/)
    : /*m_timer (io_context),*/
      m_add_started (false)
  {
  }

  void playlist_t::add (const std::string &track)
  {
    if (m_add_started == false) {
      m_add_started = true;
      m_list.clear ();
    }
    if (track.empty () == false) {
      m_list.push_back (track);
    } else {
      m_add_started = false;
    }
  }

  void playlist_t::clear ()
  {
    m_list.clear ();
  }

} // led_d
