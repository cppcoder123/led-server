//
//
//

#ifndef CONTENT_HPP
#define CONTENT_HPP

#include <list>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>
#include <utility>

#include "asio/asio.hpp"

#include "command-id.hpp"
#include "status.hpp"

namespace led_d
{

  class content_t
  {
  public:
    using status_ptr_t = std::shared_ptr<status_t>;

    content_t () = delete;
    content_t (asio::io_context &io_context,
               const std::list<std::string> &regexp_list);
    ~content_t ();

    void in (status_ptr_t status);

    using out_info_t = std::pair<std::string/*info*/, std::string/*format*/>;
    out_info_t out ();

  private:

    std::string replace (const std::string &src);

    asio::io_context &m_io_context;

    std::list<std::string> m_sys_info;

    using map_t = std::map<command_id::value_t, std::string/*info*/>;
    map_t m_info;
    map_t::iterator m_iterator;

    using regex_ptr = std::shared_ptr<std::regex>;
    using find_replace_t = std::pair<regex_ptr/*find*/, std::string/*replace*/>;
    using regex_list_t = std::list<find_replace_t>;
    regex_list_t m_regex_list;

    static const std::regex m_regex;

    std::vector<std::string> m_playlist;
  };
  
} // led_d

#endif
