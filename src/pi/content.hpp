//
//
//

#ifndef CONTENT_HPP
#define CONTENT_HPP

#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <utility>

#include "asio.hpp"

#include "command-id.hpp"
#include "command-queue.hpp"
#include "info.hpp"
#include "mcu-queue.hpp"
#include "status.hpp"
#include "status-queue.hpp"

namespace led_d
{

  class content_t
  {
  public:
    using status_ptr_t = std::shared_ptr<status_t>;

    content_t () = delete;
    content_t (const std::list<std::string> &regexp_list);
    ~content_t ();

    void to_mcu_queue (mcu_queue_t &queue) {m_to_mcu_queue = &queue;}
    void command_queue (command_queue_t &command_queue);

    void in (status_ptr_t status);

    info_t out ();

    void clear_top ();

  private:

    std::string replace (const std::string &src);

    void sync_clock (const std::string &time_src);

    mcu_queue_t *m_to_mcu_queue;

    using info_map_t = std::map<command_id_t, std::string/*info*/>;

    std::string m_top_info;               // top priority
    std::list<std::string> m_middle_info; // 
    info_map_t m_bottom_info;             // bottom priority

    info_map_t::iterator m_iterator;

    using regex_ptr = std::shared_ptr<std::regex>;
    using find_replace_t = std::pair<regex_ptr/*find*/, std::string/*replace*/>;
    using regex_list_t = std::list<find_replace_t>;
    regex_list_t m_track_regex_list;

    static const std::regex m_track_regex;
    static const std::regex m_clock_regex;
  };
  
} // led_d

#endif
