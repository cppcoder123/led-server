/*
 *
 */

#ifndef MENU_HPP
#define MENU_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace led_d
{
  class menu_t
  {
  public:

    menu_t ();
    ~menu_t () = default;

    void track_add (const std::string &track);
    void track_clear ();

    void rotor (uint8_t id, uint8_t action);

  private:

    enum class id_t {
      BRIGHTNESS,
      TRACK,
      VOLUME,
    };

    void select (id_t id);
    void select (bool inc);
    void select ();

    void value (bool inc);
    void value ();

    std::optional<id_t> m_id;
    std::optional<int> m_track;
    std::optional<int> m_volume;

    bool m_playlist_update;     // in progress
    std::vector<std::string> m_playlist;
  };

}

#endif
