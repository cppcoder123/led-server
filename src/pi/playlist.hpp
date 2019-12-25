/*
 *
 */
#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <atomic>
#include <string>
#include <vector>

#include "asio/asio.hpp"

namespace led_d
{

  class playlist_t
  {

  public:
    playlist_t (/*asio::io_context &io_context*/);
    ~playlist_t () = default;

    void add (const std::string &track);

    void clear ();

  private:
    // asio::io_context &m_io_context;
    //asio::steady_timer m_timer;
    std::vector<std::string> m_list;

    std::atomic_bool m_add_started;
  };

} // led_d


#endif
