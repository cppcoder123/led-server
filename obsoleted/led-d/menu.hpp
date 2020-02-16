/*
 *
 */

#ifndef MENU_HPP
#define MENU_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "asio/asio.hpp"

#include "command-queue.hpp"
#include "status-queue.hpp"

namespace led_d
{
  class menu_t
  {
  public:

    menu_t (asio::io_context &io_context,
            status_queue_t &status_queue);
    menu_t (const menu_t&) = delete;
    ~menu_t () = default;

    void command_queue (command_queue_t &command_queue);

    void track_add (const std::string &track);
    void track_clear ();

    void rotor (uint8_t id, uint8_t action);

    void volume_range (const std::string &low, const std::string &high);

    void current_track (const std::string &str);
    void current_volume (const std::string &str);

  private:

    enum class id_t {
      // BRIGHTNESS,
      TRACK,
      VOLUME,
    };

    void select_param (id_t id);
    void select (bool inc);
    void select ();

    void value (bool inc);
    void value ();

    void set_range ();

    void get_value ();
    void set_value ();
    void inc_value (bool direction);

    bool wrapable () const;
    bool applyable () const;

    id_t inc_id (bool inc) const;
    char id_to_letter () const;
    void display ();

    void menu_timeout (const asio::error_code &error);
    void track_timeout (const asio::error_code &error);

    static std::optional<int> to_int (const std::string &src);

    int m_delta;

    std::optional<id_t> m_id;   // track/volume/..

    using value_t = int;
    using pair_t = std::pair<value_t, value_t>;
    std::optional<pair_t> m_range;
    std::optional<value_t> m_value;

    bool m_playlist_update;     // in progress
    std::vector<std::string> m_playlist;

    std::optional<pair_t> m_volume_range;
    std::string m_volume_get;   // command
    std::string m_volume_set;   // command

    //asio::io_context &m_io_context;
    asio::steady_timer m_menu_timer;
    asio::steady_timer m_track_timer;

    command_queue_t *m_command_queue {};
    status_queue_t &m_status_queue;
  };

}

#endif
